#include <windows.h>
#include <shlobj.h>     // For IShellExtInit and IContextMenu
#include <set>
#include <string>
#include "FileProcessor.h"

class ContextMenu : public IShellExtInit, public IContextMenu 
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);

	ContextMenu(void);

protected:
    ~ContextMenu(void);


private:
    // Reference count of component.
    long m_cRef;

    // Selected files.
    FileProcessor selectedFiles;

    // The method that handles the "display" verb.
    void OnVerbDisplayFileName(HWND hWnd);


    PWSTR m_pszMenuText;
};