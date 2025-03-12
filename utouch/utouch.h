#pragma once

#include "../libutils/common_utils.h"

enum oprMode
{
    OP_FTM,  // File Time Modification
    OP_FTMR, // File Time Modification Recursively
    OP_SDT   // Directory Time Synchronization
};

struct stOprPara
{
    enum oprMode eMode;
    std::wstring fnDir;
    std::vector<std::wstring> fnFiles;
    FILETIME stTmW;
    FILETIME stTmA;
    FILETIME stTmC;
    bool bWt;
    bool bAt;
    bool bCt;
    bool bSf;
    bool bKd;
    bool bKt;
    bool bLt;
    bool bVerbose;
    std::wstring szMsg;
};

int DoUtouch(int argc, wchar_t* argv[]);