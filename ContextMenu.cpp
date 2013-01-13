#include "ContextMenu.h"
#include <strsafe.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

extern HINSTANCE g_hInst;
extern long g_cDllRef;

ContextMenu::ContextMenu(void) : m_cRef(1), m_pszMenuText(L"Avid the Best!")
{
    InterlockedIncrement(&g_cDllRef);
}

ContextMenu::~ContextMenu(void)
{
    InterlockedDecrement(&g_cDllRef);
}

void ContextMenu::OnVerbDisplayFileName(HWND hWnd)
{
	MessageBox(hWnd, selectedFiles.dumpFileInfo().c_str(), m_pszMenuText, MB_OK);
}

IFACEMETHODIMP ContextMenu::QueryInterface(REFIID riid, void **ppv)
{
    static const QITAB qit[] = 
    {
        QITABENT(ContextMenu, IContextMenu),
        QITABENT(ContextMenu, IShellExtInit), 
        { 0 },
    };
    return QISearch(this, qit, riid, ppv);
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) ContextMenu::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) ContextMenu::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef)
    {
        delete this;
    }

    return cRef;
}

// Initialize the context menu handler.
IFACEMETHODIMP ContextMenu::Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID)
{
	HRESULT hr = E_FAIL;

	if (NULL == pDataObj)
        return E_INVALIDARG;

    FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm;

	wchar_t buf[MAX_PATH];

    if (SUCCEEDED(pDataObj->GetData(&fe, &stm)))
    {
        HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (hDrop != NULL) {
            UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			if (nFiles) 
				hr = S_OK;
			for(UINT i = 0; i < nFiles; i++) 
			{
				DragQueryFile(hDrop, i, buf, ARRAYSIZE(buf));
				selectedFiles.addFile(buf);
			}
        }
		GlobalUnlock(stm.hGlobal);
        ReleaseStgMedium(&stm);
    }

    return hr;
}

IFACEMETHODIMP ContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags)
{
	// If uFlags include CMF_DEFAULTONLY then we should not do anything.
    if (CMF_DEFAULTONLY & uFlags)
    {
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));
    }

	// Menu item
    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE;
    mii.wID = idCmdFirst;
    mii.fType = MFT_STRING;
    mii.dwTypeData = m_pszMenuText;
    mii.fState = MFS_ENABLED;
    if (!InsertMenuItem(hMenu, indexMenu, TRUE, &mii))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Separator
    MENUITEMINFO sep = { sizeof(sep) };
    sep.fMask = MIIM_TYPE;
    sep.fType = MFT_SEPARATOR;
    if (!InsertMenuItem(hMenu, indexMenu + 1, TRUE, &sep))
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, 1);
}

IFACEMETHODIMP ContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
    BOOL fUnicode = FALSE;

	OnVerbDisplayFileName(pici->hwnd);

    return S_OK;
}

IFACEMETHODIMP ContextMenu::GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax)
{
    return S_OK;
}