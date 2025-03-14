#include "utouch.h"

int wmain(int argc, wchar_t* argv[])
{
    // match system's environment
    setlocale(LC_ALL, "");
    SetStdoutModeW(true);

    // do main function
    return DoUtouch(argc, argv);
}

std::wstring g_szUsage =
L"\nFile and Directory Touch Utility with Default UTC Standard v3.2.25"
L"\nCopyleft(c) 2019-2025 XHBL. This program is released under the GPL-3.0 License"
L"\n"
L"\n* File Time Modification (Default: UTC)"
L"\n   Usage: utouch [options ...] files|directory"
L"\n    /W - Change only the write time"
L"\n    /A - Change only the access time"
L"\n    /C - Change only the creation time"
L"\n    /S - Process directories (default: only files are touched)"
L"\n    /L - Use local time instead of UTC"
L"\n    /K - Keep the original date or time intact if /D or /T is not specified"
L"\n         Otherwise, the current system date or time will be used"
L"\n    /D date - Set the date in the format of mm-dd-yyyy or yyyy-mm-dd"
L"\n    /T time - Set the time in the format of hh:mm:ss (24-hour format)"
L"\n    /F file - Use timestamp from a specified reference file"
L"\n    files – List of files (including matched by wildcards) to be changed"
L"\n    /R - Recursively change all contents under a directory"
L"\n    directory – Path name of the directory to work with /R"
L"\n    /V - Verbose output with timestamp change information"
L"\n   Note: If /W /A /C are not specified, write and access time will be changed"
L"\n"
L"\n* Directory Time Synchronization"
L"\n   Usage: utouch /SD [/V] directory"
L"\n    /SD - Synchronize the directory's write time to the latest one of its"
L"\n          contained files. Process all subdirectories recursively"
L"\n    directory - Path name of the directory to work with /SD"
L"\n    /V - Verbose output with timestamp change information"
L"\n"
L"\nExamples:"
L"\n  utouch /W /A /C /D 10-05-2024 /T 18:45:38 *.log a.txt"
L"\n  utouch /W /A /C /D 10-05-2024 /T 18:45:38 /R myfolder"
L"\n  utouch /SD /V backups"
L"\n";

void UpdateUsage()
{
    std::wstring strFname, strVer;
    if (GetFileVerStrW(strFname, strVer))
    {
        StrReplaceW(g_szUsage, L"utouch", strFname);
        StrReplaceW(g_szUsage, L"3.2.25", strVer);
    }
}

int ParseOprParas(int argc, wchar_t* argv[], stOprPara& stOpr)
{
    if (argc < 2)
    {
        stOpr.szMsg = g_szUsage;
        return 1;
    }

    std::unordered_map<std::wstring, std::wstring> options;
    std::vector<std::wstring> non_options;
    bool bParseError = false;
    bool bOprDir = false;
    int yy = 0, mo = 0, dd = 0, hh = 0, mi = 0, ss = 0;
    int i = 1;
    stOpr.szMsg.clear();
    while (i < argc && !bParseError)
    {
        std::wstring param = argv[i];
        if (param.empty())
        {
            ++i;
            continue;
        }

        if (param[0] == L'/' || param[0] == L'-')
        {
            std::wstring option = StrToUpperW(param.substr(1));
            if (option == L"SD")
            {
                bOprDir = true;
                ++i;
            }
            else if (option == L"V")
            {
                options[L"V"] = L"true";
                ++i;
            }
            else
            {
                if (option == L"W")
                {
                    options[L"W"] = L"true";
                    ++i;
                }
                else if (option == L"A")
                {
                    options[L"A"] = L"true";
                    ++i;
                }
                else if (option == L"C")
                {
                    options[L"C"] = L"true";
                    ++i;
                }
                else if (option == L"K")
                {
                    options[L"K"] = L"true";
                    ++i;
                }
                else if (option == L"L")
                {
                    options[L"L"] = L"true";
                    ++i;
                }
                else if (option == L"S")
                {
                    options[L"S"] = L"true";
                    ++i;
                }
                else if (option == L"R")
                {
                    options[L"R"] = L"true";
                    ++i;
                }
                else if (option == L"D")
                {
                    if (++i >= argc)
                    {
                        stOpr.szMsg += L"Error: /D requires a date parameter.\n";
                        bParseError = true;
                    }
                    else
                    {
                        std::wstring date = argv[i];
                        if (!ValidateDateStrW(date, &yy, &mo, &dd))
                        {
                            stOpr.szMsg += L"Error: Invalid date format for /D.\n";
                            bParseError = true;
                        }
                        else
                        {
                            options[L"D"] = date;
                            ++i;
                        }
                    }
                }
                else if (option == L"T")
                {
                    if (++i >= argc)
                    {
                        stOpr.szMsg += L"Error: /T requires a time parameter.\n";
                        bParseError = true;
                    }
                    else
                    {
                        std::wstring time = argv[i];
                        if (!ValidateTimeStrW(time, &hh, &mi, &ss))
                        {
                            stOpr.szMsg += L"Error: Invalid time format for /T.\n";
                            bParseError = true;
                        }
                        else
                        {
                            options[L"T"] = time;
                            ++i;
                        }
                    }
                }
                else if (option == L"F")
                {
                    if (++i >= argc)
                    {
                        stOpr.szMsg += L"Error: /F requires a reference file parameter.\n";
                        bParseError = true;
                    }
                    else
                    {
                        std::wstring ref_file = argv[i];
                        if (ref_file.empty())
                        {
                            stOpr.szMsg += L"Error: Reference file path cannot be empty.\n";
                            bParseError = true;
                        }
                        else
                        {
                            options[L"F"] = ref_file;
                            ++i;
                        }
                    }
                }
                else
                {
                    stOpr.szMsg += L"Error: Unknown option '" + param + L"'.\n";
                    bParseError = true;
                }
            }
        }
        else
        {
            non_options.push_back(param);
            ++i;
        }
    }
    if (!bParseError)
    {
        if (bOprDir)
        {
            if (non_options.size() != 1 || non_options[0].empty())
            {
                stOpr.szMsg += L"Error: Requires exactly one path for /SD option.\n";
                bParseError = true;
            }
            if (options.count(L"W") || options.count(L"A") || options.count(L"C") || options.count(L"K") || options.count(L"L") ||
                options.count(L"S") || options.count(L"F") || options.count(L"D") || options.count(L"T") || options.count(L"R"))
            {
                stOpr.szMsg += L"Error: File-related options not valid with /SD option.\n";
                bParseError = true;
            }
        }
        else
        {
            if (non_options.empty())
            {
                stOpr.szMsg += L"Error: Requires at least one file path.\n";
                bParseError = true;
            }
            for (const auto& file : non_options)
            {
                if (file.empty())
                {
                    stOpr.szMsg += L"Error: Empty file path detected.\n";
                    bParseError = true;
                }
            }
        }
    }
    // parse error return
    if (bParseError) return 2;
    // get parameters from parsed options
    if (bOprDir)    // directory sync
    {
        stOpr.eMode = OP_SDT;
        stOpr.fnDir = non_options[0];
        if (!PathExistDir(stOpr.fnDir))
        {
            stOpr.szMsg += L"Error: Directory path '" + stOpr.fnDir + L"' not exist.\n";
            return 3;
        }
        stOpr.bVerbose = options.count(L"V") > 0;
    }
    else    // file time modification
    {
        stOpr.eMode = OP_FTM;
        if (options.count(L"R"))
        {
            stOpr.eMode = OP_FTMR;
            if (non_options.size() != 1)
            {
                stOpr.szMsg += L"Error: Requires only one directory path for /R option.\n";
                return 2;
            }
            stOpr.fnDir = non_options[0];
            if (!PathExistDir(stOpr.fnDir))
            {
                stOpr.szMsg += L"Error: Directory path '" + stOpr.fnDir + L"' not exist.\n";
                return 3;
            }
        }
        else
        {
            bool bFileExist = false;
            for (const auto& file : non_options)
            {
                stOpr.fnFiles.push_back(file);
                enPEType pet_type = PathExistType(file);
                if (pet_type != PET_NONE && pet_type != PET_WCDIRNO)
                    bFileExist = true;
                else
                    stOpr.szMsg += L"Error: File path '" + file + L"' not found.\n";
            }
            if (!bFileExist) return 3;
        }
        stOpr.bWt = options.count(L"W") > 0;
        stOpr.bAt = options.count(L"A") > 0;
        stOpr.bCt = options.count(L"C") > 0;
        if (!stOpr.bWt && !stOpr.bAt && !stOpr.bCt)
        {
            // if none of /W /A /C is specified, set /W /A by default
            stOpr.bWt = true;
            stOpr.bAt = true;
        }
        stOpr.bSf = options.count(L"S") > 0;
        stOpr.bKd = stOpr.bKt = options.count(L"K") > 0;
        stOpr.bLt = options.count(L"L") > 0;
        stOpr.bVerbose = options.count(L"V") > 0;
        SYSTEMTIME stTm;
        if (stOpr.bLt)
            GetLocalTime(&stTm);
        else
            GetSystemTime(&stTm);
        if (options.count(L"D"))
        {
            stTm.wYear = (WORD)yy;
            stTm.wMonth = (WORD)mo;
            stTm.wDay = (WORD)dd;
            stTm.wDayOfWeek = 0;
            stOpr.bKd = false;
        }
        if (options.count(L"T"))
        {
            stTm.wHour = (WORD)hh;
            stTm.wMinute = (WORD)mi;
            stTm.wSecond = (WORD)ss;
            stTm.wMilliseconds = 0;
            stOpr.bKt = false;
        }
        FILETIME stFtm;
        if (!SystemTimeToFileTime(&stTm, &stFtm))
        {
            stOpr.szMsg += L"Input time may be invalid.\n";
            return 2;
        }
        if (stOpr.bLt)
        {
            FILETIME stFtmUTC;
            LocalFileTimeToFileTime(&stFtm, &stFtmUTC);
            stFtm = stFtmUTC;
        }
        if (options.count(L"F"))
        {
            std::wstring ref_file = options[L"F"];
            HANDLE hFile = CreateFile(ref_file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
            if (hFile == INVALID_HANDLE_VALUE)
            {
                stOpr.szMsg += L"Refrence file path '" + ref_file + L"' is invalid.\n";
                return 3;
            }
            if (!GetFileTime(hFile, &stOpr.stTmC, &stOpr.stTmA, &stOpr.stTmW))
            {
                stOpr.szMsg += L"Refrence file '" + ref_file + L"' time can't be accessed.\n";
                CloseHandle(hFile);
                return 4;
            }
            CloseHandle(hFile);
        }
        else
        {
            stOpr.stTmW = stFtm;
            stOpr.stTmA = stFtm;
            stOpr.stTmC = stFtm;
        }
    }
    return 0;
}

std::wstring FormatFileTime(const FILETIME& ft, bool toLocal, bool appendUTC)
{
    SYSTEMTIME st, stl;
    if (!FileTimeToSystemTime(&ft, &st))
        return L"INVALID_TIME";
    if (toLocal && SystemTimeToTzSpecificLocalTime(NULL, &st, &stl))
        st = stl;
    wchar_t buffer[32];
    swprintf_s(buffer, L"%04d-%02d-%02d %02d:%02d:%02d%s",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond,
        !toLocal && appendUTC ? L" UTC" : L"");
    return buffer;
}

void KeepPartialDateTime(FILETIME* pstTmSet, const FILETIME* pstTmOrg, BOOL bKd, BOOL bKt)
{
    if (!bKd && !bKt) return;
    if (!pstTmSet || !pstTmOrg) return;

    SYSTEMTIME stmSet, stmOrg;
    FileTimeToSystemTime(pstTmSet, &stmSet);
    FileTimeToSystemTime(pstTmOrg, &stmOrg);
    if (bKd)
    {
        stmSet.wYear = stmOrg.wYear;
        stmSet.wMonth = stmOrg.wMonth;
        stmSet.wDay = stmOrg.wDay;
    }
    if (bKt)
    {
        stmSet.wHour = stmOrg.wHour;
        stmSet.wMinute = stmOrg.wMinute;
        stmSet.wSecond = stmOrg.wSecond;
        stmSet.wMilliseconds = stmOrg.wMilliseconds;
    }
    SystemTimeToFileTime(&stmSet, pstTmSet);
}

std::int64_t FTtoint64(const FILETIME& ft)
{
    return (static_cast<std::int64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
}

void int64toFT(FILETIME& ft, std::int64_t ftm)
{
    ft.dwLowDateTime = static_cast<DWORD>(ftm & 0xFFFFFFFF);
    ft.dwHighDateTime = static_cast<DWORD>(ftm >> 32);
}

bool UtouchFTMFile(const std::wstring& fullPath, const WIN32_FIND_DATAW& wfd, stOprPara& stOpr)
{
    bool isDir = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    HANDLE hFile = CreateFileW(fullPath.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
        isDir ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL, NULL);
    bool bSuccess = false;
    if (hFile != INVALID_HANDLE_VALUE)
    {
        // adjust time if required
        FILETIME tmpW = stOpr.stTmW, tmpA = stOpr.stTmA, tmpC = stOpr.stTmC;
        if (stOpr.bWt) KeepPartialDateTime(&tmpW, &wfd.ftLastWriteTime, stOpr.bKd, stOpr.bKt);
        if (stOpr.bAt) KeepPartialDateTime(&tmpA, &wfd.ftLastAccessTime, stOpr.bKd, stOpr.bKt);
        if (stOpr.bCt) KeepPartialDateTime(&tmpC, &wfd.ftCreationTime, stOpr.bKd, stOpr.bKt);
        bSuccess = SetFileTime(hFile, stOpr.bCt ? &tmpC : NULL, stOpr.bAt ? &tmpA : NULL, stOpr.bWt ? &tmpW : NULL);
        CloseHandle(hFile);
        // verbose output
        if (stOpr.bVerbose)
        {
            std::wstring flags;
            FILETIME displayTime = { 0 };
            if (stOpr.bWt) { flags += L'W'; displayTime = tmpW; }
            if (stOpr.bCt) { flags += L'C'; if (flags.length() == 1) displayTime = tmpC; }
            if (stOpr.bAt) { flags += L'A'; if (flags.length() == 1) displayTime = tmpA; }
            std::wcout << L"[" << flags << L" -> " << FormatFileTime(displayTime, stOpr.bLt, true)
                << L" " << (bSuccess ? L"OK" : L"FAIL") << L"] " << fullPath << std::endl;
        }
    }
    if (!bSuccess)
    {
        std::wstring opitem = isDir ? L"directory: " : L"file: ";
        stOpr.szMsg += L"Failed to touch " + opitem + fullPath + L"\n";
    }
    return bSuccess;
}

int UtouchFTM(stOprPara& stOpr)
{
    bool bAllSuccess = true;
    for (const auto& ff : stOpr.fnFiles)
    {
        WIN32_FIND_DATAW wfd;
        std::wstring pattern = ff;
        PathNormalizeW(pattern, true);   // remove trailing slashes
        HANDLE hFind = FindFirstFileW(pattern.c_str(), &wfd);
        if (hFind == INVALID_HANDLE_VALUE) {
            stOpr.szMsg += L"File not found: " + pattern + L"\n";
            bAllSuccess = false;
            continue;
        }
        // find all files
        do {
            // skip directories if not required
            bool isDir = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            if (isDir && (!stOpr.bSf || wcscmp(wfd.cFileName, L".") == 0 || wcscmp(wfd.cFileName, L"..") == 0))
                continue;
            // get full path
            size_t pos = pattern.find_last_of(L"\\/:");
            std::wstring fullPath = (pos != std::wstring::npos) ? pattern.substr(0, pos + 1) + wfd.cFileName : wfd.cFileName;
            // in case of drive letter "C:"
            if (pattern.size() == 2 && pattern[1] == L':') fullPath = pattern;
            // touch file
            if (!UtouchFTMFile(fullPath, wfd, stOpr))
                bAllSuccess = false;
        } while (FindNextFileW(hFind, &wfd));
        FindClose(hFind);
    }
    return bAllSuccess ? 0 : 5;
}

void UtouchFTMRDir(const std::wstring& dirPath, stOprPara& stOpr, bool& bAllSuccess)
{
    std::wstring searchPath = PathAppendFileW(dirPath, L"*");
    WIN32_FIND_DATAW wfd;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &wfd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        stOpr.szMsg += L"Failed to search directory: " + dirPath + L"\n";
        bAllSuccess = false;
        return;
    }
    do
    {
        if (wcscmp(wfd.cFileName, L".") == 0 || wcscmp(wfd.cFileName, L"..") == 0) continue;
        std::wstring fullPath = PathAppendFileW(dirPath, wfd.cFileName);
        bool isDir = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        // process file
        if (!isDir || stOpr.bSf)
        {
            if (!UtouchFTMFile(fullPath, wfd, stOpr))
                bAllSuccess = false;
        }
        // process sub-directory
        if (isDir) UtouchFTMRDir(fullPath, stOpr, bAllSuccess);
    } while (FindNextFileW(hFind, &wfd));
    FindClose(hFind);
}

int UtouchFTMR(stOprPara& stOpr)
{
    bool bAllSuccess = true;
    std::wstring rootDir = stOpr.fnDir;
    PathNormalizeW(rootDir, true);
    // touch files recursively
    UtouchFTMRDir(rootDir, stOpr, bAllSuccess);
    // root directory to be touched if required
    if (stOpr.bSf)
    {
        rootDir = stOpr.fnDir;
        PathNormalizeW(rootDir, true, true);
        bool bDriveRoot = !rootDir.empty() && rootDir.back() == L'\\';
        if (!bDriveRoot)    // skip drive root like "C:\" or "\"
        {
            WIN32_FIND_DATAW wfdRoot;
            HANDLE hRoot = FindFirstFileW(rootDir.c_str(), &wfdRoot);
            if (hRoot != INVALID_HANDLE_VALUE)
            {
                bool isDir = (wfdRoot.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
                if (isDir && !UtouchFTMFile(rootDir, wfdRoot, stOpr))
                    bAllSuccess = false;
            }
            else
            {
                stOpr.szMsg += L"Directory not found: " + rootDir + L"\n";
                bAllSuccess = false;
            }
            FindClose(hRoot);
        }
    }
    return bAllSuccess ? 0 : 5;
}

bool UtouchSDTFile(const std::wstring& fullPath, const WIN32_FIND_DATAW& wfd, stOprPara& stOpr)
{
    bool isDir = (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    HANDLE hFile = CreateFileW(fullPath.c_str(), GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
        isDir ? FILE_FLAG_BACKUP_SEMANTICS : FILE_ATTRIBUTE_NORMAL, NULL);
    bool bSuccess = false;
    if (hFile != INVALID_HANDLE_VALUE)
    {
        // adjust time if required
        FILETIME setFt = { 0 };
        int64toFT(setFt, stOpr.nTmSD);
        bSuccess = SetFileTime(hFile, NULL, NULL, &setFt);
        CloseHandle(hFile);
        // verbose output
        if (stOpr.bVerbose)
        {
            std::wcout << L"[" << FormatFileTime(wfd.ftLastWriteTime, true, false) << L" -> " << FormatFileTime(setFt, true, false)
                << L" " << (bSuccess ? L"OK" : L"FAIL") << L"] " << fullPath << std::endl;
        }
    }
    if (!bSuccess)
    {
        std::wstring opitem = isDir ? L"directory: " : L"file: ";
        stOpr.szMsg += L"Failed to touch " + opitem + fullPath + L"\n";
    }
    return bSuccess;
}

std::int64_t UtouchSDTDir(const std::wstring& dirPath, stOprPara& stOpr, bool& bAllSuccess)
{
    std::wstring searchPath = PathAppendFileW(dirPath, L"*");
    WIN32_FIND_DATAW wfd;
    HANDLE hFind = FindFirstFileW(searchPath.c_str(), &wfd);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        stOpr.szMsg += L"Failed to search directory: " + dirPath + L"\n";
        bAllSuccess = false;
        return 0;
    }
    std::int64_t lastFileTime = 0;
    std::int64_t lastDirTime = 0;
    do
    {
        if (wcscmp(wfd.cFileName, L".") == 0 || wcscmp(wfd.cFileName, L"..") == 0) continue;
        std::int64_t wfdTime = FTtoint64(wfd.ftLastWriteTime);
        std::wstring fullPath = PathAppendFileW(dirPath, wfd.cFileName);
        // process sub-directory
        if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            std::int64_t subTime = UtouchSDTDir(fullPath, stOpr, bAllSuccess);
            bool success = false;
            if (subTime > 0 && subTime < wfdTime)
            {
                stOpr.nTmSD = subTime;
                success = UtouchSDTFile(fullPath, wfd, stOpr);
            }
            std::int64_t tmpTime = success ? subTime : wfdTime;
            if (subTime > 0 && tmpTime > lastDirTime)
                lastDirTime = tmpTime;
        }
        else
            lastFileTime = wfdTime > lastFileTime ? wfdTime : lastFileTime;
    } while (FindNextFileW(hFind, &wfd));
    FindClose(hFind);
    return lastFileTime > lastDirTime ? lastFileTime : lastDirTime;
}

int UtouchSDT(stOprPara& stOpr)
{
    bool bAllSuccess = true;
    std::wstring rootDir = stOpr.fnDir;
    PathNormalizeW(rootDir, true);
    // process recursively
    std::int64_t subTime = UtouchSDTDir(rootDir, stOpr, bAllSuccess);
    // root directory to be processed if required
    rootDir = stOpr.fnDir;
    PathNormalizeW(rootDir, true, true);
    bool bDriveRoot = !rootDir.empty() && rootDir.back() == L'\\';
    if (!bDriveRoot)    // skip drive root like "C:\" or "\"
    {
        WIN32_FIND_DATAW wfdRoot;
        HANDLE hRoot = FindFirstFileW(rootDir.c_str(), &wfdRoot);
        if (hRoot != INVALID_HANDLE_VALUE)
        {
            std::int64_t wfdTime = FTtoint64(wfdRoot.ftLastWriteTime);
            bool isDir = (wfdRoot.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            if (isDir && subTime > 0 && subTime < wfdTime)
            {
                stOpr.nTmSD = subTime;
                if (!UtouchSDTFile(rootDir, wfdRoot, stOpr))
                    bAllSuccess = false;
            }
        }
        else
        {
            stOpr.szMsg += L"Directory not found: " + rootDir + L"\n";
            bAllSuccess = false;
        }
        FindClose(hRoot);
    }
    return bAllSuccess ? 0 : 6;
}

// core function
int DoUtouch(int argc, wchar_t* argv[])
{
    UpdateUsage();

    // parse parameters
    stOprPara oprPara;
    int iRet = ParseOprParas(argc, argv, oprPara);
    if (iRet)
    {
        if (iRet == 1) std::wcout << oprPara.szMsg; // show usage
        else std::wcerr << oprPara.szMsg; // show error
        return iRet;
    }

    // do operation
    if (oprPara.eMode == OP_FTM)
        iRet = UtouchFTM(oprPara);
    else if (oprPara.eMode == OP_FTMR)
        iRet = UtouchFTMR(oprPara);
    else if (oprPara.eMode == OP_SDT)
        iRet = UtouchSDT(oprPara);

    // output message
    if (iRet) std::wcerr << oprPara.szMsg;
    return iRet;
}