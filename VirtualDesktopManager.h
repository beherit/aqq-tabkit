//---------------------------------------------------------------------------
// Based on the investigation by NickoTin, source code of
// "Wrapper for API to Windows 10 Virtual Desktop" and "VDMHelper"
// http://www.cyberforum.ru/blogs/105416/blog3671.html
// https://github.com/Grabacr07/VirtualDesktop
// https://github.com/tmyt/VDMHelper
//---------------------------------------------------------------------------
#pragma once

const TGUID CLSID_VirtualDesktopManager = StringToGUID("{aa509086-5ca9-4c25-8f95-589d3c07b48a}");

MIDL_INTERFACE("a5cd92ff-29be-454c-8d04-d82879fb3f1b")
IVirtualDesktopManager : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(
		/* [in] */ __RPC__in HWND topLevelWindow,
		/* [out] */ __RPC__out BOOL *onCurrentDesktop) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(
		/* [in] */ __RPC__in HWND topLevelWindow,
		/* [out] */ __RPC__out GUID *desktopId) = 0;

	virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(
		/* [in] */ __RPC__in HWND topLevelWindow,
		/* [in] */ __RPC__in REFGUID desktopId) = 0;
};