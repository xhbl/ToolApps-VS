#pragma once

#include <stdio.h>
#include "btlcks_util.h"

#include "resource.h"


#define BLUPLEN   16
#define BLHDRLEN  (BLUPLEN*2)
#define BLRKLEN   1024
#define BLMSGLEN  1024

enum BLAct
{
    BLA_NA = 0,
    BLA_UK,
    BLA_CK,
    BLA_DK,
    BLA_CS,
};

class CBtlck
{
public:
    CBtlck();
    ~CBtlck();

    int Init();
    int Run();
    int DoUK();
    int DoCK();
    int DoDK();
    int DoCS();
    void MsgBox();

private:
    bool LoadCK();
    bool SaveCK();
    void CalCK();
    void CalDK();
    void InitPFN();
    void InitUP();
    bool IsForCU();
    bool IsMatchPP();

    void SetMsgHelp();
    void SetMsg(const TCHAR* cap, const TCHAR* fmt, ...);
    bool IsPFExist(LPCTSTR szPFN, bool bDir);
    int GetDriveEncryptionStatus(LPCTSTR parsingName);

    TCHAR m_szDrv[3];

    TCHAR m_szFdApp[_MAX_PATH + 1], m_szFdDat[_MAX_PATH + 1];
    TCHAR m_szFnApp[_MAX_FNAME + 1], m_szFnDrv[_MAX_FNAME + 1];
    TCHAR m_szPnApp[_MAX_PATH + 1], m_szPnCky[_MAX_PATH + 1];

    unsigned char* m_dbtrk, * m_cbtrk;
    unsigned int m_lenbtrk;
    char m_sXK[BLUPLEN + 1], m_sCU[BLUPLEN + 1], m_sPP[BLUPLEN + 1];

    bool m_bLK, m_bUK;
    enum BLAct m_act;

    TCHAR* m_szMsg;
    TCHAR* m_szMsgCap;
    bool m_bMsg;
    int m_iMsgLevel;
};
