// btlck.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "btlcks.h"
#include "AES.h"

#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "propsys.lib")


void CBtlck::SetMsgHelp()
{
    TCHAR sTlt[128], sApp[128];
    _tcscpy_s(sApp, 128, m_szFnApp);
    _tcsupr_s(sApp, 128);
    _stprintf_s(sTlt, 128, _T("%s (%s) - Usage Help"), sApp, sizeof(void*) == 4 ? _T("x86") : _T("x64"));
    SetMsg(sTlt, _T(
        "BitLocker Drive Lock/Unlock Switcher v%s\n"
        "Copyleft(c) 2022-2025 XHBL. This program is released under the GPL-3.0 License\n"
        "\n"
        "Usage: %s DriveLetter [Options]\n"
        "\n"
        "  DriveLetter: Specifies the drive letter (A-Z)\n"
        "  Options:\n"
        "    -ck pwd rkey: Creates a security credential from the recovery key\n"
        "        pwd - Password used to encrypt the security credential\n"
        "        rkey - BitLocker recovery key string\n"
        "    -dk pwd: Displays the recovery key from the security credential\n"
        "        pwd - Password used to decrypt the security credential\n"
        "    -cs : Creates a desktop shortcut for the specified drive\n"
        "    -lk: Locks the drive\n"
        "    -uk: Unlocks the drive\n"
        "    (no option): Toggles the drive's lock/unlock status\n"
        "\n"
        "Example Steps:\n"
        " 1. Create a security credential: '%s D -ck mypwd 473671-...-308156'\n"
        " 2. Create a desktop shortcut for quick toggling: '%s D -cs'\n"
        " 3. Click the shortcut to lock or unlock the drive\n"),
        CBtUtil::GetVerString(), m_szFnApp, m_szFnApp, m_szFnApp, m_szFnApp, m_szFnApp);
    m_iMsgLevel = 0;
}

CBtlck::CBtlck()
{
    m_szDrv[0] = 0;

    m_szFdApp[0] = m_szFdDat[0] = 0;
    m_szFnApp[0] = m_szFnDrv[0] = 0;
    m_szPnApp[0] = 0;
    m_szPnCky[0] = 0;

    m_dbtrk = m_cbtrk = NULL;
    m_lenbtrk = 0;

    m_sXK[0] = m_sCU[0] = m_sPP[0] = 0;

    m_bLK = m_bUK = false;

    m_act = BLAct::BLA_NA;

    m_szMsg = new TCHAR[BLMSGLEN];
    m_szMsgCap = new TCHAR[BLMSGLEN];
    m_szMsg[0] = m_szMsgCap[0] = 0;
    m_bMsg = false;
    m_iMsgLevel = 0;
}

CBtlck::~CBtlck()
{
    delete[] m_dbtrk;
    delete[] m_cbtrk;
    delete[] m_szMsg;
    delete[] m_szMsgCap;
}

int CBtlck::Init()
{
    int     iArgc = 0;
    LPWSTR* sArgv = CommandLineToArgvW(GetCommandLineW(), &iArgc);

    if (1 == iArgc)
    {
        InitPFN();
        SetMsgHelp();
        m_act = BLAct::BLA_NA;
        return 0;
    }

    if ((1 < iArgc && wcslen(sArgv[1]) != 1) || !isalpha((char)sArgv[1][0]))
        goto errArg;

    m_szDrv[0] = _totupper(sArgv[1][0]);
    m_szDrv[1] = ':';
    m_szDrv[2] = 0;

    InitPFN();
    InitUP();

    if (2 == iArgc)
    {
        m_act = BLAct::BLA_UK;
        return 0;
    }

    if (3 == iArgc)
    {
        if (0 == _tcsicmp(sArgv[2], _T("-lk")))
        {
            m_act = BLAct::BLA_UK;
            m_bLK = true;
            return 0;
        }
        if (0 == _tcsicmp(sArgv[2], _T("-uk")))
        {
            m_act = BLAct::BLA_UK;
            m_bUK = true;
            return 0;
        }
        if (0 == _tcsicmp(sArgv[2], _T("-cs")))
        {
            m_act = BLAct::BLA_CS;
            return 0;
        }
    }

    if (4 == iArgc)
    {
        if (0 == _tcsicmp(sArgv[2], _T("-dk")))
        {
            int i, plen = (int)_tcslen(sArgv[3]);
            if (plen > BLUPLEN)
            {
                SetMsg(_T("Invalid Para"), _T("Password too long, be no more than %d chars."), BLUPLEN);
                m_iMsgLevel = 2;
                return 1;
            }
            for (i = 0; i < plen; i++)
                m_sPP[i] = (char)sArgv[3][i];
            m_act = BLAct::BLA_DK;
            return 0;
        }
    }

    if (5 == iArgc)
    {
        if (0 == _tcsicmp(sArgv[2], _T("-ck")))
        {
            int plen = (int)_tcslen(sArgv[3]);
            if (plen > BLUPLEN)
            {
                SetMsg(_T("Invalid Para"), _T("Password too long, be no more than %d chars."), BLUPLEN);
                m_iMsgLevel = 2;
                return 1;
            }
            int rlen = (int)_tcslen(sArgv[4]);
            if (rlen > BLRKLEN)
            {
                SetMsg(_T("Invalid Para"), _T("Recovery key too long."));
                m_iMsgLevel = 2;
                return 1;
            }
            int i = BLHDRLEN + rlen;
            m_lenbtrk = i % 16 ? (i / 16 + 1) * 16 : i;
            m_dbtrk = new unsigned char[m_lenbtrk + 1];
            unsigned char* pd = m_dbtrk;
            for (i = 0; i < BLUPLEN; i++, pd++)
                *pd = m_sCU[i];
            for (i = 0; i < BLUPLEN; i++, pd++)
            {
                if (i < plen) m_sPP[i] = (char)sArgv[3][i];
                *pd = m_sPP[i];
            }
            for (i = 0; i < (int)m_lenbtrk - BLHDRLEN; i++, pd++)
            {
                if (i < rlen) *pd = (unsigned char)sArgv[4][i];
                else *pd = 0;
            }
            m_dbtrk[m_lenbtrk] = 0;
            m_cbtrk = new unsigned char[m_lenbtrk + 1];
            memset(m_cbtrk, 0, m_lenbtrk + 1);
            m_act = BLAct::BLA_CK;
            return 0;
        }
    }

errArg:
    SetMsg(_T("Wrong usage"), _T("Invalid command arguments.\nPlease read the usage help."));
    m_iMsgLevel = 2;
    return 1;
}

int CBtlck::Run()
{
    int iRet = 0;
    switch (m_act)
    {
    case BLAct::BLA_UK:
        iRet = DoUK();
        break;
    case BLAct::BLA_CK:
        iRet = DoCK();
        break;
    case BLAct::BLA_DK:
        iRet = DoDK();
        break;
    case BLAct::BLA_CS:
        iRet = DoCS();
        break;
    default:
        break;
    }
    return iRet;
}

int CBtlck::DoUK()
{
    bool bUseRK = false;
    TCHAR szRK[BLRKLEN + 1];
    szRK[0] = 0;
    if (LoadCK())
    {
        CalDK();
        if (IsForCU())
        {
            bUseRK = true;
            int len = m_lenbtrk - BLHDRLEN > BLRKLEN ? BLRKLEN : m_lenbtrk - BLHDRLEN;
            for (int i = 0; i < len; i++)
                szRK[i] = (TCHAR)m_dbtrk[i + BLHDRLEN];
            szRK[len] = 0;
        }
    }

    bool bShow = true;

    PVOID OldValue = NULL;
    Wow64DisableWow64FsRedirection(&OldValue);

    TCHAR szBtLckCmd[256], szBtLckPar[2048];
    GetSystemDirectory(szBtLckCmd, 256);
    _tcscat_s(szBtLckCmd, 256, _T("\\manage-bde.exe"));
    if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(szBtLckCmd))
    {
        return 10;
    }

    int iDrvStatus = GetDriveEncryptionStatus(m_szDrv);
    if (iDrvStatus == 1)
    {
        if (m_bUK)
            return 0;
        _tcscpy_s(szBtLckPar, 2048, _T("-lock -forcedismount "));
        _tcscat_s(szBtLckPar, 2048, m_szDrv);
        bShow = false;
    }
    else if (iDrvStatus == 6)
    {
        if (m_bLK)
            return 0;
        _tcscpy_s(szBtLckPar, 2048, _T("-unlock "));
        _tcscat_s(szBtLckPar, 2048, m_szDrv);
        if (bUseRK)
        {
            _tcscat_s(szBtLckPar, 2048, _T(" -RecoveryPassword "));
            _tcscat_s(szBtLckPar, 2048, szRK);
            bShow = false;
        }
        else
            _tcscat_s(szBtLckPar, 2048, _T(" -Password"));
    }
    else
    {
        return 11;
    }

    SHELLEXECUTEINFO ShExecInfo = { 0 };
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = _T("runas");
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = bShow ? SW_SHOW : SW_HIDE;
    ShExecInfo.hInstApp = NULL;

    BOOL bElevated = CBtUtil::IsRunElevated();
    if (bElevated)
    {
        ShExecInfo.lpFile = szBtLckCmd;
        ShExecInfo.lpParameters = szBtLckPar;
    }
    else
    {
        ShExecInfo.lpFile = m_szPnApp;
        ShExecInfo.lpParameters = CBtUtil::GetCmdParas();
    }

    if (!ShellExecuteEx(&ShExecInfo))
    {
        return 12;
    }

    DWORD dwRet = 0;
    if (bElevated && ShExecInfo.hProcess)
    {
        WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
        GetExitCodeProcess(ShExecInfo.hProcess, &dwRet);
    }
    return dwRet;
}

int CBtlck::DoCK()
{
    CalCK();
    if (SaveCK())
    {
        SetMsg(_T("Create security credential to"), m_szPnCky);
        m_iMsgLevel = 1;
        return 0;
    }
    SetMsg(_T("Error"), _T("Create security credential failed.\n%s"), m_szPnCky);
    m_iMsgLevel = 2;
    return 2;
}

int CBtlck::DoDK()
{
    if (LoadCK())
    {
        CalDK();
        if (IsMatchPP())
        {
            TCHAR* szRK = new TCHAR[m_lenbtrk];
            for (int i = 0; i < (int)m_lenbtrk - BLHDRLEN; i++)
                szRK[i] = (TCHAR)m_dbtrk[i + BLHDRLEN];
            szRK[m_lenbtrk - BLHDRLEN] = 0;
            SetMsg(_T("Recovery key is"), szRK);
            m_iMsgLevel = 1;
            delete[] szRK;
            return 0;
        }
        else
        {
            SetMsg(_T("Invalid password"), _T("Wrong password to access security credential."));
            m_iMsgLevel = 2;
            return 3;
        }
    }

    SetMsg(_T("Error"), _T("Can not access security credential."));
    m_iMsgLevel = 2;
    return 2;
}

int CBtlck::DoCS()
{
    TCHAR szPnLnk[_MAX_PATH + 1], szPnLnk2[_MAX_PATH + 1];
    LPCTSTR szFdDesk = CBtUtil::GetDesktopPath();
    if (szFdDesk)
    {
        _tcscpy_s(szPnLnk, _MAX_PATH, szFdDesk);
        _tcscat_s(szPnLnk, _MAX_PATH, _T("\\"));
        _tcscpy_s(szPnLnk2, _MAX_PATH, m_szFdDat);
    }
    else
    {
        _tcscpy_s(szPnLnk, _MAX_PATH, m_szFdDat);
        _tcscpy_s(szPnLnk2, _MAX_PATH, m_szFdApp);
    }
    _tcscat_s(szPnLnk, _MAX_PATH, m_szFnDrv);
    _tcscat_s(szPnLnk, _MAX_PATH, _T(".lnk"));
    _tcscat_s(szPnLnk2, _MAX_PATH, m_szFnDrv);
    _tcscat_s(szPnLnk2, _MAX_PATH, _T(".lnk"));

    TCHAR szPar[2];
    szPar[0] = m_szDrv[0];
    szPar[1] = 0;

    int icoIdx = szPar[0] - 'A' + 1;

    int iRet = CBtUtil::CreateShortcut(m_szPnApp, szPar, szPnLnk, _T(""), SW_SHOW, m_szFdApp, m_szPnApp, icoIdx);
    if (0 == iRet)
    {
        CopyFile(szPnLnk, szPnLnk2, FALSE);
        SetMsg(_T("Create shortcut to"), _T("%s\n%s"), szPnLnk, szPnLnk2);
        m_iMsgLevel = 1;
    }
    else
    {
        SetMsg(_T("Error"), _T("Failed to create shortcut. Error code: %i\n%s"), iRet, szPnLnk);
        m_iMsgLevel = 2;
    }
    return iRet;
}

void CBtlck::SetMsg(const TCHAR* cap, const TCHAR* fmt, ...)
{
    if (cap)
        _tcscpy_s(m_szMsgCap, BLMSGLEN, cap);
    else
        m_szMsgCap[0] = 0;

    if (fmt)
    {
        va_list argptr;
        va_start(argptr, fmt);
        _vstprintf_s(m_szMsg, BLMSGLEN, fmt, argptr);
        va_end(argptr);
        m_bMsg = true;
    }
    else
    {
        m_szMsg[0] = 0;
        m_bMsg = false;
    }
}

void CBtlck::MsgBox()
{
    if (m_bMsg)
    {
        UINT uType = MB_OK;
        if (1 == m_iMsgLevel) uType = MB_ICONINFORMATION;
        if (2 == m_iMsgLevel) uType = MB_ICONERROR;
        MessageBox(NULL, m_szMsg, m_szMsgCap, uType);
    }
}

bool CBtlck::LoadCK()
{
    FILE* fp = NULL;
    _tfopen_s(&fp, m_szPnCky, _T("r"));
    if (NULL == fp)
        return false;

    int lbuf = BLHDRLEN + BLRKLEN;
    unsigned char* rbuf = new unsigned char[lbuf];
    int lrd = (int)fread_s(rbuf, lbuf, sizeof(unsigned char), lbuf, fp);
    fclose(fp);

    if (lrd < BLHDRLEN)
    {
        delete[] rbuf;
        return false;
    }

    m_lenbtrk = lrd % 16 ? (lrd / 16 + 1) * 16 : lrd;
    m_cbtrk = new unsigned char[m_lenbtrk + 1];
    memset(m_cbtrk, 0, m_lenbtrk + 1);
    memcpy(m_cbtrk, rbuf, lrd);
    delete[] rbuf;

    m_dbtrk = new unsigned char[m_lenbtrk + 1];
    memset(m_dbtrk, 0, m_lenbtrk + 1);

    return true;
}

bool CBtlck::SaveCK()
{
    if (!m_lenbtrk)
    {
        return false;
    }

    FILE* fp = NULL;
    _tfopen_s(&fp, m_szPnCky, _T("w"));
    if (NULL == fp)
        return false;

    int lwt = (int)fwrite(m_cbtrk, sizeof(unsigned char), m_lenbtrk, fp);
    fclose(fp);

    if (lwt == m_lenbtrk)
        return true;
    return false;
}

void CBtlck::CalCK()
{
    AES aes(AESKeyLength::AES_128);
    unsigned char* ct = aes.EncryptECB(m_dbtrk, m_lenbtrk, (unsigned char*)m_sXK);
    if (ct)
    {
        memcpy(m_cbtrk, ct, m_lenbtrk);
        delete[] ct;
    }
}

void CBtlck::CalDK()
{
    AES aes(AESKeyLength::AES_128);
    unsigned char* dt = aes.DecryptECB(m_cbtrk, m_lenbtrk, (unsigned char*)m_sXK);
    if (dt)
    {
        memcpy(m_dbtrk, dt, m_lenbtrk);
        delete[] dt;
    }
}

void CBtlck::InitPFN()
{
    if (!GetModuleFileName(NULL, m_szPnApp, _MAX_PATH))
        m_szPnApp[0] = 0;

    TCHAR szDir[_MAX_DIR + 1];
    _tsplitpath_s(m_szPnApp, m_szFdApp, _MAX_DRIVE, szDir, _MAX_DIR, m_szFnApp, _MAX_FNAME, NULL, 0);
    _tcscat_s(m_szFdApp, _MAX_PATH, szDir);
    _tcscpy_s(m_szFnDrv, _MAX_FNAME, m_szFnApp);
    _tcsncat_s(m_szFnDrv, _MAX_FNAME, m_szDrv, 1);

    LPCTSTR szFdDat = CBtUtil::GetAppdataPath();
    if (szFdDat)
    {
        _tcscpy_s(m_szFdDat, _MAX_PATH, szFdDat);
        _tcscat_s(m_szFdDat, _MAX_PATH, _T("\\"));
        _tcscat_s(m_szFdDat, _MAX_PATH, m_szFnApp);
        _tcscat_s(m_szFdDat, _MAX_PATH, _T("\\"));
        if (!IsPFExist(m_szFdDat, true))
            CreateDirectory(m_szFdDat, NULL);
        if (IsPFExist(m_szFdDat, true))
            _tcscpy_s(m_szPnCky, _MAX_PATH, m_szFdDat);
    }
    else
    {
        _tcscpy_s(m_szFdDat, _MAX_PATH, m_szFdApp);
        _tcscpy_s(m_szPnCky, _MAX_PATH, m_szFdApp);
    }

    _tcscat_s(m_szPnCky, _MAX_PATH, m_szFnDrv);
    _tcscat_s(m_szPnCky, _MAX_PATH, _T(".cky"));
}

void CBtlck::InitUP()
{
    memset(m_sXK, 0, BLUPLEN + 1);
    m_sXK[10] = 0x79;   m_sXK[0] = 0x7E;     m_sXK[14] = 0x63;   m_sXK[9] = 0x65;
    m_sXK[1] = 0x62;    m_sXK[13] = 0x4C;    m_sXK[5] = 0x74;    m_sXK[2] = 0x55;
    m_sXK[6] = 0x49;    m_sXK[15] = 0x4B;    m_sXK[11] = 0x42;   m_sXK[4] = 0x4C;
    m_sXK[3] = 0x69;    m_sXK[12] = 0x74;    m_sXK[8] = 0x4B;    m_sXK[7] = 0x6E;

    memset(m_sCU, 0, BLUPLEN + 1);
    char sCU[BLUPLEN + 1];
    DWORD dwLen = BLUPLEN + 1;
    if (GetUserNameA(sCU, &dwLen))
    {
        _strlwr_s(sCU, dwLen);
        memcpy(m_sCU, sCU, dwLen);
    }

    memset(m_sPP, 0, BLUPLEN + 1);
}

bool CBtlck::IsForCU()
{
    if (m_lenbtrk < BLHDRLEN)
        return false;

    char sKU[BLUPLEN + 1];
    memcpy(sKU, m_dbtrk, BLUPLEN);
    sKU[BLUPLEN] = 0;

    if (0 == strcmp(sKU, m_sCU))
        return true;
    return false;
}

bool CBtlck::IsMatchPP()
{
    if (m_lenbtrk < BLHDRLEN)
        return false;

    char sPP[BLUPLEN + 1];
    memcpy(sPP, m_dbtrk + BLUPLEN, BLUPLEN);
    sPP[BLUPLEN] = 0;

    if (0 == strcmp(sPP, m_sPP))
        return true;
    return false;
}

bool CBtlck::IsPFExist(LPCTSTR szPFN, bool bDir)
{
    DWORD dwAttrib = GetFileAttributes(szPFN);
    if (dwAttrib == INVALID_FILE_ATTRIBUTES)
        return false;
    if (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
        return bDir ? true : false;
    else
        return bDir ? false : true;
}

// status value ( 1, 3, 5, 6 consider as BitLocker enabled )
// 1: BitLocker on          2: BitLocker off
// 3: BitLocker encrypting  4: BitLocker decrypting
// 5: BitLocker suspended   6: BitLocker on (locked)
// 8: BitLocker waiting for activation
int CBtlck::GetDriveEncryptionStatus(LPCTSTR parsingName)
{
    int status = 0;
    IShellItem2* drive = NULL;
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    hr = SHCreateItemFromParsingName(parsingName, NULL, IID_PPV_ARGS(&drive));
    if (SUCCEEDED(hr)) {
        PROPERTYKEY pKey;
        hr = PSGetPropertyKeyFromName(_T("System.Volume.BitLockerProtection"), &pKey);
        if (SUCCEEDED(hr)) {
            PROPVARIANT prop;
            PropVariantInit(&prop);
            hr = drive->GetProperty(pKey, &prop);
            if (SUCCEEDED(hr)) {
                status = prop.intVal;

                drive->Release();
                if (status == 1 || status == 3 || status == 5)
                    return status;
                else
                    return status;
            }
        }
    }

    if (drive)
        drive->Release();

    return status;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(nCmdShow);

    int iRet = 0;
    CBtlck btlck;

    if (iRet = btlck.Init())
    {
        btlck.MsgBox();
        return iRet;
    }

    iRet = btlck.Run();
    btlck.MsgBox();
    return iRet;
}
