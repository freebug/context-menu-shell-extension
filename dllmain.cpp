// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "ClassFactory.h"
#include "RegistryHelper.h"
#include <windows.h>
#include <Guiddef.h>

const PCWSTR pszFriendlyName = L"ContextMenuHelper";

// {46C519D1-C210-424C-BF8B-3FE0C87786D6}
static const CLSID CLSID_FileContextMenuExt = 
{ 0x46c519d1, 0xc210, 0x424c, { 0xbf, 0x8b, 0x3f, 0xe0, 0xc8, 0x77, 0x86, 0xd6 } };

HINSTANCE   g_hInst     = NULL;
long        g_cDllRef   = 0;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
        g_hInst = hModule;
        DisableThreadLibraryCalls(hModule);
        break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
    if (IsEqualCLSID(CLSID_FileContextMenuExt, rclsid))
    {
        hr = E_OUTOFMEMORY;	
        
		// Create the COM component.
        ClassFactory *pClassFactory = new ClassFactory();
        if (pClassFactory)
        {
            // Query the specified interface.
            hr = pClassFactory->QueryInterface(riid, ppv);
            pClassFactory->Release();
        }
    }

    return hr;
}

STDAPI DllCanUnloadNow(void)
{
    return g_cDllRef > 0 ? S_FALSE : S_OK;
}

STDAPI DllRegisterServer(void)
{
    HRESULT hr;

	wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    // Register the component.
	hr = RegisterInprocServer(szModule, CLSID_FileContextMenuExt, pszFriendlyName, L"Apartment");
	hr = RegisterShellExtContextMenuHandler(pszFriendlyName, CLSID_FileContextMenuExt);
    
	return hr;
}

STDAPI DllUnregisterServer(void)
{
    HRESULT hr = S_OK;
    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }
    // Unregister the component.
    hr = UnregisterInprocServer(CLSID_FileContextMenuExt);
    if (SUCCEEDED(hr))
    {
        hr = UnregisterShellExtContextMenuHandler(pszFriendlyName, CLSID_FileContextMenuExt);
    }

    return hr;
}