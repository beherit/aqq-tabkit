//---------------------------------------------------------------------------
// Copyright (C) 2010-2013 Krzysztof Grochocki
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

#define IOCTL_KEYBOARD_SET_INDICATORS		CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0002, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KEYBOARD_QUERY_TYPEMATIC		CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0008, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_KEYBOARD_QUERY_INDICATORS		CTL_CODE(FILE_DEVICE_KEYBOARD, 0x0010, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _KEYBOARD_INDICATOR_PARAMETERS
{
  USHORT UnitId;
  USHORT LedFlags;
} KEYBOARD_INDICATOR_PARAMETERS, *PKEYBOARD_INDICATOR_PARAMETERS;

#define KEYBOARD_CAPS_LOCK_ON		4
#define KEYBOARD_NUM_LOCK_ON		2
#define KEYBOARD_SCROLL_LOCK_ON		1

int FlashKeyboardLight(HANDLE hKbdDev, UINT LightFlag, int Duration);
HANDLE OpenKeyboardDevice(int *ErrorNumber);
int CloseKeyboardDevice(HANDLE hndKbdDev);
HANDLE FlashKeyboardLightInThread(UINT LightFlag, int Duration, LPCTSTR EventName);

typedef struct
{
  UINT LightFlag;
  int Duration;
  wchar_t EventName[128];
} FLASH_KBD_THD_INIT, *LPFLASH_KBD_THD_INIT;
