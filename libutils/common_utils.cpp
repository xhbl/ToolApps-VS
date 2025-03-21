﻿// common_utils.cpp : This file contains the commonly used utility functions.
//

#include "common_utils.h"

void SetStdoutModeW(bool bSet)
{
    static int svmode = -1;
    static bool isSet = false;
    if (bSet)
    {
        if (!isSet)
        {
            int cmode = _setmode(_fileno(stdout), _O_U16TEXT);
            if (cmode != -1)
            {
                svmode = cmode;
                isSet = true;
            }
        }
    }
    else
    {
        if (isSet)
        {
            if (_setmode(_fileno(stdout), svmode) != -1)
                isSet = false;
        }
    }
}

int ConPrintfW(const wchar_t* fmt, ...)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return -1;
    // get formatted length
    va_list args;
    va_start(args, fmt);
    va_list args_copy;
    va_copy(args_copy, args);
    int length = _vscwprintf(fmt, args_copy);
    va_end(args_copy);
    if (length < 0) { va_end(args); return -1; }
    // format to buffer
    size_t bufferSize = static_cast<size_t>(length) + 1;
    std::unique_ptr<wchar_t[]> buffer(new wchar_t[bufferSize]);
    int written = _vsnwprintf_s(buffer.get(), bufferSize, _TRUNCATE, fmt, args);
    va_end(args);
    if (written != length) return -1;
    // output to console
    DWORD charsWritten = 0;
    BOOL success = WriteConsoleW(hConsole, buffer.get(), written, &charsWritten, nullptr);
    if (!success) return -1;
    return static_cast<int>(charsWritten);
}

void StrReplaceW(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
    if (from.empty()) return;
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::wstring::npos)
    {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

std::wstring StrFormatW(const wchar_t* fmt, ...)
{
    if (!fmt)
        return std::wstring();

    va_list args;
    va_start(args, fmt);

    // Copy argument list (for length calculation)
    va_list args_copy;
    va_copy(args_copy, args);
    // First call: get formatted length (excluding null terminator)
    const int len = _vscwprintf(fmt, args_copy);
    va_end(args_copy);
    if (len <= 0)
    {
        va_end(args);
        return std::wstring();
    }

    // Allocate buffer (length + null terminator)
    std::vector<wchar_t> buffer(len + 1);
    // Second call: write formatted content
    const int written = std::vswprintf(buffer.data(), buffer.size(), fmt, args);
    va_end(args);
    if (written < 1)
        return std::wstring();
    return std::wstring(buffer.data(), written);
}

std::wstring StrFormatW(const std::wstring fmt, ...)
{
    if (fmt.empty()) return std::wstring();
    va_list args;
    va_start(args, fmt);
    std::wstring result = StrFormatW(fmt, args);
    va_end(args);
    return result;
}

size_t StriFindW(const std::wstring& str, const std::wstring& substr, size_t start_pos)
{
    if (substr.empty()) {
        return start_pos <= str.size() ? start_pos : std::wstring::npos;
    }
    if (start_pos > str.size() || str.size() - start_pos < substr.size()) {
        return std::wstring::npos;
    }
    // Iterate through each possible starting position in the main string
    for (size_t i = start_pos; i <= str.size() - substr.size(); ++i) {
        bool match = true;
        // Check if current position matches the substring
        for (size_t j = 0; j < substr.size(); ++j) {
            if (std::towlower(str[i + j]) != std::towlower(substr[j])) {
                match = false;
                break;
            }
        }
        if (match) return i; // Match found, return current position
    }
    return std::wstring::npos; // No match found
}

void StrTrimW(std::wstring& str, const wchar_t* tgt, int mode)
{
    static const wchar_t default_whitespace[] = L" \t\n\r\f\v";
    const wchar_t* trim_chars = (tgt == nullptr || *tgt == L'\0') ? default_whitespace : tgt;

    switch (mode)
    {
    case 0: // Trim both head&end
    {
        size_t first = str.find_first_not_of(trim_chars);
        if (first == std::wstring::npos)
        {
            str.clear();
            return;
        }
        size_t last = str.find_last_not_of(trim_chars);
        str = str.substr(first, (last - first + 1));
        break;
    }

    case 1: // // Trim left
    {
        size_t first = str.find_first_not_of(trim_chars);
        if (first != std::wstring::npos)
            str.erase(0, first);
        else
            str.clear();
        break;
    }

    case 2: // Trim right
    {
        size_t last = str.find_last_not_of(trim_chars);
        if (last != std::wstring::npos)
            str.erase(last + 1);
        else
            str.clear();
        break;
    }

    default:
        break;
    }
}

void StrTrimW(std::wstring& str, wchar_t chtgt, int mode)
{
    wchar_t tgt[2] = { chtgt, L'\0' };
    StrTrimW(str, tgt, mode);
}

std::wstring StrToUpperW(const std::wstring& str)
{
    std::wstring result;
    for (wchar_t c : str)
        result.push_back(std::towupper(c));
    return result;
}

std::wstring StrToLowerW(const std::wstring& str)
{
    std::wstring result;
    for (wchar_t c : str)
        result.push_back(std::towlower(c));
    return result;
}

int NumOfChars(const std::wstring& strTgt, wchar_t chFN)
{
    int nCount = 0;
    size_t nLocate = 0;

    while ((nLocate = strTgt.find(chFN, nLocate)) != std::wstring::npos)
    {
        nCount++;
        nLocate++;
    }

    return nCount;
}

void WStrToAnsi(char* pcDst, const wchar_t* pcSrc)
{
    int iLen = wcslen(pcSrc);
    int iClen = ::WideCharToMultiByte(
        CP_ACP,         // code page
        0,              // performance and mapping flags
        pcSrc,          // address of wide-character string
        iLen,           // number of characters in string
        pcDst,          // address of buffer for new string
        iLen * 2,       // size of buffer
        NULL,
        NULL);
    pcDst[iClen] = 0;
}

std::string WStrToUtf8(const wchar_t* pcSrc)
{
    if (!pcSrc || *pcSrc == L'\0')
        return std::string();
    int len = static_cast<int>(wcslen(pcSrc));
    int size = WideCharToMultiByte(CP_UTF8, 0, pcSrc, len, nullptr, 0, nullptr, nullptr);
    std::string utf8str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, pcSrc, len, &utf8str[0], size, nullptr, nullptr);
    return utf8str;
}

std::wstring NormalizeToCRLF(const std::wstring& input)
{
    std::wstring result;
    size_t crlf_count = std::count_if(input.begin(), input.end(),
        [](wchar_t c) { return c == L'\r' || c == L'\n'; });

    result.reserve(input.length() + crlf_count);
    for (size_t i = 0; i < input.length(); ++i)
    {
        if (input[i] == L'\r')
        {
            result += L"\r\n";
            if (i + 1 < input.length() && input[i + 1] == L'\n') ++i;
        }
        else if (input[i] == L'\n')
        {
            result += L"\r\n";
            if (i + 1 < input.length() && input[i + 1] == L'\r') ++i;
        }
        else
        {
            result += input[i];
        }
    }
    return result;
}

bool ValidateDateStrW(const std::wstring& date_str, int* pyy, int* pmm, int* pdd)
{
    std::wregex pattern(L"^(?:"
        L"(\\d{4})[-/](\\d{1,2})[-/](\\d{1,2})|"    // fmt1: YYYY-MM-DD or YYYY/MM/DD
        L"(\\d{1,2})[-/](\\d{1,2})[-/](\\d{4}))$"   // fmt2: MM-DD-YYYY or MM/DD/YYYY
    );
    std::wsmatch match;
    if (!std::regex_match(date_str, match, pattern)) return false;
    // extract and convert values
    int year = 0, month = 0, day = 0;
    if (!match[1].str().empty())    // fmt1
    {
        year = std::stoi(match[1]); month = std::stoi(match[2]); day = std::stoi(match[3]);
    }
    else    // fmt2
    {
        year = std::stoi(match[6]); month = std::stoi(match[4]); day = std::stoi(match[5]);
    }
    // validate
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    static const std::array<int, 13> days_in_month =
    {
        0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    bool is_leap = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
    int max_day = days_in_month[month];
    if (month == 2 && is_leap) max_day = 29;
    if (day > max_day) return false;
    // output results
    if (pyy != nullptr) *pyy = year;
    if (pmm != nullptr) *pmm = month;
    if (pdd != nullptr) *pdd = day;
    return true;
}

bool ValidateTimeStrW(const std::wstring& time_str, int* phh, int* pmm, int* pss, int* pSSS)
{
    // hh:mm:ss[.SSS] format
    std::wregex pattern(L"^([01]?\\d|2[0-3]):([0-5]?\\d):([0-5]?\\d)(?:\\.(\\d{1,3}))?$");
    std::wsmatch match;
    if (!std::regex_match(time_str, match, pattern)) return false;
    // extract and convert values
    int hours = std::stoi(match[1]);
    int minutes = std::stoi(match[2]);
    int seconds = std::stoi(match[3]);
    int milliseconds = 0;
    if (match[4].matched) milliseconds = std::stoi(match[4]);
    // validate
    if (hours < 0 || hours > 23) return false;
    if (minutes < 0 || minutes > 59) return false;
    if (seconds < 0 || seconds > 59) return false;
    if (milliseconds < 0 || milliseconds > 999) return false;
    // output results
    if (phh != nullptr) *phh = hours;
    if (pmm != nullptr) *pmm = minutes;
    if (pss != nullptr) *pss = seconds;
    if (pSSS != nullptr) *pSSS = milliseconds;
    return true;
}

size_t ReadOneLine(const std::wstring& strText, std::wstring& strLine, size_t pos)
{
    strLine.clear();
    if (pos >= strText.length())
        return std::wstring::npos;

    size_t startPos = pos;
    size_t endPos = pos;
    while (endPos < strText.length())
    {
        wchar_t currentChar = strText[endPos];
        if (currentChar == L'\r' || currentChar == L'\n')
        {
            strLine = strText.substr(startPos, endPos - startPos);
            if (currentChar == L'\r' && endPos + 1 < strText.length() && strText[endPos + 1] == L'\n')
                return endPos + 2; // "\r\n"
            else if (currentChar == L'\n' && endPos + 1 < strText.length() && strText[endPos + 1] == L'\r')
                return endPos + 2; // "\n\r"
            else
                return endPos + 1; // '\r' or '\n'
        }
        endPos++;
    }
    strLine = strText.substr(startPos, endPos - startPos);
    return std::wstring::npos; // no more content
}

int ReadTextFileW(const wchar_t* filename, std::wstring& output, size_t maxrlen)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
        return GetLastError();

    // Get file size
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    // Determine buffer size
    maxrlen = std::min<size_t>(maxrlen, fileSize);
    std::vector<char> buffer(maxrlen);
    file.read(buffer.data(), maxrlen);
    size_t bytesRead = static_cast<size_t>(file.gcount());
    file.close();

    // Check for BOM (Byte Order Mark)
    int encoding = 0; // 0: ANSI, 1: UTF-8, 2: UTF-16 LE, 3: UTF-16 BE
    size_t bomLength = 0;
    if (bytesRead >= 3 && buffer[0] == '\xEF' && buffer[1] == '\xBB' && buffer[2] == '\xBF')
    {
        encoding = 1; // UTF-8 with BOM
        bomLength = 3;
    }
    else if (bytesRead >= 2)
    {
        if (buffer[0] == '\xFF' && buffer[1] == '\xFE')
        {
            encoding = 2; // UTF-16 LE
            bomLength = 2;
        }
        else if (buffer[0] == '\xFE' && buffer[1] == '\xFF')
        {
            encoding = 3; // UTF-16 BE
            bomLength = 2;
        }
    }
    // UTF-8 detection without BOM
    if (encoding == 0)
    {
        bool isUtf8 = true;
        size_t utf8CharsFound = 0;
        // Check up to 100 UTF-8 characters
        for (size_t i = 0; i < bytesRead && utf8CharsFound < 100; ++i)
        {
            if ((buffer[i] & 0x80) != 0)
            {
                int followingBytes = 0;
                if ((buffer[i] & 0xE0) == 0xC0)
                    followingBytes = 1;
                else if ((buffer[i] & 0xF0) == 0xE0)
                    followingBytes = 2;
                else if ((buffer[i] & 0xF8) == 0xF0)
                    followingBytes = 3;
                else
                {
                    isUtf8 = false;
                    break;
                }
                for (int j = 0; j < followingBytes; ++j)
                {
                    if (++i >= bytesRead || (buffer[i] & 0xC0) != 0x80)
                    {
                        isUtf8 = false;
                        break;
                    }
                }
                if (!isUtf8) break;
                utf8CharsFound++;
            }
        }
        if (isUtf8 && utf8CharsFound > 0)
            encoding = 1; // UTF-8 without BOM
    }

    // Convert content based on encoding
    char* dataStart = buffer.data() + bomLength;
    size_t dataLength = (bytesRead >= bomLength) ? (bytesRead - bomLength) : 0;
    int result = 0;
    if (encoding == 1) // UTF-8
    {
        int wcharsNeeded = MultiByteToWideChar(CP_UTF8, 0, dataStart, dataLength, nullptr, 0);
        if (wcharsNeeded > 0)
        {
            output.resize(wcharsNeeded);
            MultiByteToWideChar(CP_UTF8, 0, dataStart, dataLength, &output[0], wcharsNeeded);
        }
        else
            result = GetLastError();
    }
    else if (encoding == 2) // UTF-16 LE
    {
        output.assign(reinterpret_cast<const wchar_t*>(dataStart), dataLength / sizeof(wchar_t));
    }
    else if (encoding == 3) // UTF-16 BE
    {
        for (size_t i = 0; i < dataLength / 2 * 2; i += 2)
            std::swap(dataStart[i], dataStart[i + 1]); // swap bytes for BE to LE
        output.assign(reinterpret_cast<const wchar_t*>(dataStart), dataLength / sizeof(wchar_t));
    }
    else // ANSI or unknown
    {
        int wcharsNeeded = MultiByteToWideChar(CP_ACP, 0, dataStart, dataLength, nullptr, 0);
        if (wcharsNeeded > 0)
        {
            output.resize(wcharsNeeded);
            MultiByteToWideChar(CP_ACP, 0, dataStart, dataLength, &output[0], wcharsNeeded);
        }
        else
            result = GetLastError();
    }

    return result;
}

enum enPEType PathExistType(const std::wstring& path)
{
    if (path.empty()) return PET_NONE;
    const bool hasWildcard = path.find_first_of(L"*?") != std::wstring::npos;
    if (hasWildcard)
    {
        WIN32_FIND_DATAW findData;
        HANDLE hFind = ::FindFirstFileW(path.c_str(), &findData);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            ::FindClose(hFind);
            return PET_WCEXIST;
        }
        const size_t lastSep = path.find_last_of(L"\\/");
        const std::wstring dirPart = (lastSep != std::wstring::npos) ? path.substr(0, lastSep) : L"";
        const std::wstring filePart = (lastSep != std::wstring::npos) ? path.substr(lastSep + 1) : path;
        if (dirPart.find_first_of(L"*?") != std::wstring::npos)
            return PET_NONE;
        if (dirPart.empty())
            return PET_WCDIRNA;
        else
        {
            const DWORD dirAttrs = ::GetFileAttributesW(dirPart.c_str());
            if (dirAttrs != INVALID_FILE_ATTRIBUTES && (dirAttrs & FILE_ATTRIBUTE_DIRECTORY))
                return PET_WCDIROK;
            else
                return PET_WCDIRNO;
        }
    }
    else
    {
        const DWORD attrs = ::GetFileAttributesW(path.c_str());
        if (attrs == INVALID_FILE_ATTRIBUTES)
            return PET_NONE;
        if (attrs & FILE_ATTRIBUTE_DIRECTORY)
            return PET_DIR;
        else
            return PET_FILE;
    }
}

bool PathExistFile(const std::wstring& path)
{
    return PathExistType(path) == PET_FILE;
}

bool PathExistDir(const std::wstring& path)
{
    return PathExistType(path) == PET_DIR;
}

void PathNormalizeW(std::wstring& path, bool keeprootslash, bool normalizeslash)
{
    StrTrimW(path);
    if (path.empty()) return;
    bool is_slashroot = path.find_last_not_of(L"/\\") == std::wstring::npos;
    if (is_slashroot) path.resize(keeprootslash ? 1 : 0);
    else
    {
        bool is_driveroot = path.size() >= 3 && path[1] == L':' && path.find_last_not_of(L"/\\") == 1 &&
            ((path[0] >= L'A' && path[0] <= L'Z') || (path[0] >= L'a' && path[0] <= L'z'));
        if (is_driveroot) path.resize(keeprootslash ? 3 : 2);
        else
            while (!path.empty() && (path.back() == L'/' || path.back() == L'\\')) path.pop_back();
    }
    if(normalizeslash) std::replace(path.begin(), path.end(), '/', '\\');
}

std::wstring PathAppendFileW(const std::wstring& path, const std::wstring& file)
{
    if (file.empty()) return path;
    std::wstring slashNeed = L"\\";
    if (path.empty() || path.back() == L'\\' || path.back() == L'/' || (path.size() == 2 && path[1] == L':'))
        slashNeed.clear(); // current path with no need slash appended
    return path + slashNeed + file;
}

bool GetFileVerStrW(std::wstring& fnStr, std::wstring& verStr)
{
    wchar_t filePath[MAX_PATH];
    DWORD dwSize = GetModuleFileName(nullptr, filePath, MAX_PATH);
    if (dwSize == 0) return false;

    // Extract filename without path and extension
    std::wstring fullFileName(filePath);
    size_t pos = fullFileName.find_last_of(L"\\/");
    fnStr = (pos == std::wstring::npos) ? fullFileName : fullFileName.substr(pos + 1);
    pos = fnStr.find_last_of(L'.');
    if (pos != std::wstring::npos)
        fnStr = fnStr.substr(0, pos);

    // Load version retrieval functions
    HMODULE hModule = LoadLibrary(L"version.dll");
    if (!hModule) return false;
    typedef DWORD(WINAPI* PFN_GETFILEVERSIONINFOSIZE)(LPWSTR, LPDWORD);
    typedef BOOL(WINAPI* PFN_GETFILEVERSIONINFO)(LPWSTR, DWORD, DWORD, LPBYTE);
    typedef BOOL(WINAPI* PFN_VERQUERYVALUE)(LPVOID, LPCWSTR, LPVOID*, PUINT);
    PFN_GETFILEVERSIONINFOSIZE pGetFileVersionInfoSize =
        (PFN_GETFILEVERSIONINFOSIZE)GetProcAddress(hModule, "GetFileVersionInfoSizeW");
    PFN_GETFILEVERSIONINFO pGetFileVersionInfo =
        (PFN_GETFILEVERSIONINFO)GetProcAddress(hModule, "GetFileVersionInfoW");
    PFN_VERQUERYVALUE pVerQueryValue =
        (PFN_VERQUERYVALUE)GetProcAddress(hModule, "VerQueryValueW");
    if (!pGetFileVersionInfoSize || !pGetFileVersionInfo || !pVerQueryValue)
    {
        FreeLibrary(hModule);
        return false;
    }
    DWORD dwHandle = 0;
    DWORD dwFileSize = pGetFileVersionInfoSize(filePath, &dwHandle);
    if (dwFileSize == 0)
    {
        FreeLibrary(hModule);
        return false;
    }
    // Read version information
    bool bRet = false;
    BYTE* versionData = new BYTE[dwFileSize];
    if (pGetFileVersionInfo(filePath, dwHandle, dwFileSize, versionData))
    {
        VS_FIXEDFILEINFO* fileInfo = nullptr;
        UINT size = 0;
        if (pVerQueryValue(versionData, L"\\", (LPVOID*)&fileInfo, &size))
        {
            if (fileInfo)
            {
                DWORD majorVersion = HIWORD(fileInfo->dwFileVersionMS);
                DWORD minorVersion = LOWORD(fileInfo->dwFileVersionMS);
                DWORD buildNumber = HIWORD(fileInfo->dwFileVersionLS);
                DWORD revisionNumber = LOWORD(fileInfo->dwFileVersionLS);
                verStr = std::to_wstring(majorVersion) + L"." +
                    std::to_wstring(minorVersion) + L"." +
                    std::to_wstring(buildNumber) + L"." +
                    std::to_wstring(revisionNumber);
                bRet = true;
            }
        }
    }
    delete[] versionData;
    FreeLibrary(hModule);
    return bRet;
}
