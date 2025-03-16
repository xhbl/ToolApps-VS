#include <tchar.h>
#include <windows.h>
#include <shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>

#include "btlcks_util.h"

#pragma comment(lib, "version.lib")


HRESULT CBtUtil::CreateShortcut(LPCTSTR pszTargetfile, LPCTSTR pszTargetargs,
    LPCTSTR pszLinkfile, LPCTSTR pszDescription,
    int iShowmode, LPCTSTR pszCurdir,
    LPCTSTR pszIconfile, int iIconindex)
{
    HRESULT       hRes;          /* Returned COM result code */
    IShellLink* pShellLink;      /* IShellLink object pointer */
    IPersistFile* pPersistFile;  /* IPersistFile object pointer */
    WCHAR wszLinkfile[MAX_PATH]; /* pszLinkfile as Unicode string */

    CoInitialize(NULL);
    hRes = E_INVALIDARG;
    if (
        (pszTargetfile != NULL) && (_tcslen(pszTargetfile) > 0) &&
        (pszTargetargs != NULL) &&
        (pszLinkfile != NULL) && (_tcslen(pszLinkfile) > 0) &&
        (pszDescription != NULL) &&
        (iShowmode >= 0) &&
        (pszCurdir != NULL) &&
        (pszIconfile != NULL) &&
        (iIconindex >= 0)
        )
    {
        hRes = CoCreateInstance(
            CLSID_ShellLink,         /* pre-defined CLSID of the IShellLink object */
            NULL,                    /* pointer to parent interface if part of aggregate */
            CLSCTX_INPROC_SERVER,    /* caller and called code are in same process */
            IID_IShellLink,          /* pre-defined interface of the IShellLink object */
            (LPVOID*)&pShellLink);   /* Returns a pointer to the IShellLink object */
        if (SUCCEEDED(hRes))
        {
            /* Set the fields in the IShellLink object */
            hRes = pShellLink->SetPath(pszTargetfile);
            hRes = pShellLink->SetArguments(pszTargetargs);
            if (_tcslen(pszDescription) > 0)
            {
                hRes = pShellLink->SetDescription(pszDescription);
            }
            if (iShowmode > 0)
            {
                hRes = pShellLink->SetShowCmd(iShowmode);
            }
            if (_tcslen(pszCurdir) > 0)
            {
                hRes = pShellLink->SetWorkingDirectory(pszCurdir);
            }
            if (_tcslen(pszIconfile) > 0 && iIconindex >= 0)
            {
                hRes = pShellLink->SetIconLocation(pszIconfile, iIconindex);
            }

            /* Use the IPersistFile object to save the shell link */
            hRes = pShellLink->QueryInterface(
                IID_IPersistFile,          /* pre-defined interface of the                                              IPersistFile object */
                (LPVOID*)&pPersistFile);   /* returns a pointer to the IPersistFile object */
            if (SUCCEEDED(hRes))
            {
                if (1 == sizeof(TCHAR))
                    MultiByteToWideChar(CP_ACP, 0, (LPCCH)pszLinkfile, -1, wszLinkfile, MAX_PATH);
                else
                    wcscpy_s(wszLinkfile, MAX_PATH, (wchar_t*)pszLinkfile);
                hRes = pPersistFile->Save(wszLinkfile, TRUE);
                pPersistFile->Release();
            }
            pShellLink->Release();
        }

    }
    CoUninitialize();
    return (hRes);
}

LPCTSTR CBtUtil::GetDesktopPath()
{
    static TCHAR szPath[MAX_PATH + 1];

    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, szPath)))
    {
        return szPath;
    }
    else
        return NULL;
}

LPCTSTR CBtUtil::GetAppdataPath()
{
    static TCHAR szPath[MAX_PATH + 1];

    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, szPath)))
    {
        return szPath;
    }
    else
        return NULL;
}

BOOL CBtUtil::IsRunElevated()
{
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = Elevation.TokenIsElevated;
        }
    }
    if (hToken) {
        CloseHandle(hToken);
    }
    return fRet;
}

LPCTSTR CBtUtil::GetCmdParas()
{
    LPTSTR pCmd = GetCommandLine();
    if (!pCmd)
        return _T("");
    if (*pCmd == '"')
    {
        pCmd++;
        while (*pCmd && *pCmd != '"') pCmd++;
    }
    while (*pCmd && *pCmd != ' ') pCmd++;
    while (*pCmd && *pCmd == ' ') pCmd++;
    return pCmd;
}

LPCTSTR CBtUtil::GetVerString()
{
    static TCHAR szVer[64 + 1];
    szVer[0] = 0;

    // get the filename of the executable containing the version resource
    TCHAR szFilename[MAX_PATH + 1] = { 0 };
    if (GetModuleFileName(NULL, szFilename, MAX_PATH) == 0)
        return szVer;

    // allocate a block of memory for the version info
    DWORD dummy;
    DWORD dwSize = GetFileVersionInfoSize(szFilename, &dummy);
    if (dwSize == 0)
        return szVer;

    BYTE* data = new BYTE[dwSize];

    // load the version info
    if (!GetFileVersionInfo(szFilename, NULL, dwSize, &data[0]))
    {
        delete[] data;
        return szVer;
    }

    // get version strings
    UINT                uiVerLen = 0;
    VS_FIXEDFILEINFO* pFixedInfo = 0;     // pointer to fixed file info structure
    // get the fixed file info (language-independent) 
    if (VerQueryValue(&data[0], TEXT("\\"), (void**)&pFixedInfo, (UINT*)&uiVerLen))
    {
        _stprintf_s(szVer, 64, _T("%u.%u.%u.%u"),
            HIWORD(pFixedInfo->dwFileVersionMS),
            LOWORD(pFixedInfo->dwFileVersionMS),
            HIWORD(pFixedInfo->dwFileVersionLS),
            LOWORD(pFixedInfo->dwFileVersionLS));
    }
    delete[] data;
    return szVer;
}
