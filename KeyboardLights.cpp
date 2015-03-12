//---------------------------------------------------------------------------
// Copyright (C) 2010-2014 Krzysztof Grochocki
//
// This file is part of TabKit
//
// TabKit is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// TabKit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

#include <windows.h>
#include <winioctl.h>
#pragma hdrstop
#include "KeyboardLights.h"

//Miganie diodami LED
int FlashKeyboardLight(HANDLE hKbdDev, UINT LedFlag, int Duration)
{
	KEYBOARD_INDICATOR_PARAMETERS InputBuffer;
	KEYBOARD_INDICATOR_PARAMETERS OutputBuffer;
	UINT LedFlagsMask;
	BOOL Toggle;
	ULONG DataLength = sizeof(KEYBOARD_INDICATOR_PARAMETERS);
	ULONG ReturnedLength;
	int i;

	InputBuffer.UnitId = 0;
	OutputBuffer.UnitId = 0;

	if(!DeviceIoControl(hKbdDev, IOCTL_KEYBOARD_QUERY_INDICATORS,&InputBuffer, DataLength, &OutputBuffer, DataLength,	&ReturnedLength, NULL))
		return GetLastError();

	LedFlagsMask = (OutputBuffer.LedFlags & (~LedFlag));

	Toggle = (OutputBuffer.LedFlags & LedFlag);

	for(i=0;i<2;i++)
	{
		Toggle ^= 1;
		InputBuffer.LedFlags = (LedFlagsMask | (LedFlag * Toggle));

		if(!DeviceIoControl(hKbdDev, IOCTL_KEYBOARD_SET_INDICATORS,	&InputBuffer, DataLength, NULL, 0, &ReturnedLength, NULL))
			return GetLastError();

		Sleep(Duration);
	}

	return 0;
}
//---------------------------------------------------------------------------

//Otwarcie sterownika klawiatury
HANDLE OpenKeyboardDevice(int *ErrorNumber)
{
	HANDLE hndKbdDev;
	int *LocalErrorNumber;
	int Dummy;

	if(ErrorNumber==NULL)
		LocalErrorNumber = &Dummy;
	else
		LocalErrorNumber = ErrorNumber;

	*LocalErrorNumber = 0;

	if(!DefineDosDevice(DDD_RAW_TARGET_PATH, L"Kbd",L"\\Device\\KeyboardClass0"))
	{
		*LocalErrorNumber = GetLastError();
		return INVALID_HANDLE_VALUE;
	}

	hndKbdDev = CreateFile(L"\\\\.\\Kbd", GENERIC_WRITE, 0, NULL,	OPEN_EXISTING, 0, NULL);

	if(hndKbdDev==INVALID_HANDLE_VALUE)
		*LocalErrorNumber = GetLastError();

	return hndKbdDev;
}
//---------------------------------------------------------------------------

//Zamkniecie sterownika klawiatury
int CloseKeyboardDevice(HANDLE hndKbdDev)
{
	int LastErrorCode = 0;

	if(!DefineDosDevice(DDD_REMOVE_DEFINITION, L"Kbd", NULL))
		LastErrorCode = GetLastError();

	if(!CloseHandle(hndKbdDev))
		LastErrorCode = GetLastError();

	return LastErrorCode;
}
//---------------------------------------------------------------------------

//Watek migania diod LED
DWORD WINAPI FlashKeyboardLightThd(LPVOID lpv)
{
	LPFLASH_KBD_THD_INIT pInit = (LPFLASH_KBD_THD_INIT)lpv;
	FLASH_KBD_THD_INIT Init = *pInit;
	HANDLE	hndKbdDev;
	HANDLE	heventCancel = OpenEvent(EVENT_ALL_ACCESS, FALSE, Init.EventName);

	if(heventCancel==NULL) ExitThread(-1);

	hndKbdDev = OpenKeyboardDevice(NULL);
	if(hndKbdDev==INVALID_HANDLE_VALUE)
	{
		CloseHandle(heventCancel);
		ExitThread(-1);
	}

	for(;;)
	{
		FlashKeyboardLight(hndKbdDev, Init.LightFlag, Init.Duration);

		if(WaitForSingleObject(heventCancel, Init.Duration) != WAIT_TIMEOUT)
			break;
	}

	Sleep(Init.Duration);

	CloseHandle(heventCancel);
	CloseKeyboardDevice(hndKbdDev);

	ExitThread(0);
	return 0;
}
//---------------------------------------------------------------------------

//Tworzenie watku z miganiem diod LED
HANDLE FlashKeyboardLightInThread(UINT LightFlag, int Duration, LPCTSTR EventName)
{
	DWORD ThreadId;
	static FLASH_KBD_THD_INIT FlashInit;

	FlashInit.LightFlag = LightFlag;
	FlashInit.Duration = Duration;
	lstrcpyn(FlashInit.EventName, EventName, 128);

	return CreateThread(NULL, 0, FlashKeyboardLightThd, (LPVOID)&FlashInit, 0, &ThreadId);
}
//---------------------------------------------------------------------------
