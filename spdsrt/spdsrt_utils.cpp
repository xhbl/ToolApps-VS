// spdsrt_utils.cpp : This file contains the commonly used utility functions.
//

#include "spdsrt_utils.h"

void StrReplaceW(std::wstring& str, const std::wstring& from, const std::wstring& to)
{
    if (from.empty()) return; // 避免死循环
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::wstring::npos)
    {
        str.replace(pos, from.length(), to);
        pos += to.length(); // // 移动索引，防止死循环
    }
}

std::wstring StrFormatW(const wchar_t* fmt, ...)
{
    if (!fmt)
        return std::wstring();

    va_list args;
    va_start(args, fmt);

    // 复制参数列表（用于计算长度）
    va_list args_copy;
    va_copy(args_copy, args);
    // 第一次调用：获取格式化后的长度（不含终止符）
    const int len = _vscwprintf(fmt, args_copy);
    va_end(args_copy);
    if (len <= 0)
    {
        va_end(args);
        return std::wstring();
    }

    // 分配缓冲区（长度+终止符）
    std::vector<wchar_t> buffer(len + 1);
    // 第二次调用：实际写入格式化内容
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
    // 遍历主字符串的每个可能的起始位置
    for (size_t i = start_pos; i <= str.size() - substr.size(); ++i) {
        bool match = true;
        // 检查当前位置是否匹配子字符串
        for (size_t j = 0; j < substr.size(); ++j) {
            if (std::towlower(str[i + j]) != std::towlower(substr[j])) {
                match = false;
                break;
            }
        }
        if (match) {
            return i; // 找到匹配，返回当前位置
        }
    }
    return std::wstring::npos; // 未找到匹配
}

void StrTrimW(std::wstring& str, const wchar_t* tgt, int mode)
{
    static const wchar_t default_whitespace[] = L" \t\n\r\f\v";
    const wchar_t* trim_chars = (tgt == nullptr || *tgt == L'\0') ? default_whitespace : tgt;

    switch (mode)
    {
    case 0: // Trim 首尾
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

    case 1: // Trim 左侧
    {
        size_t first = str.find_first_not_of(trim_chars);
        if (first != std::wstring::npos)
            str.erase(0, first);
        else
            str.clear();
        break;
    }

    case 2: // Trim 右侧
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

int NumOfChars(const std::wstring& strTgt, wchar_t chFN)
{
    int nCount = 0;
    size_t nLocate = 0;

    while ((nLocate = strTgt.find(chFN, nLocate)) != std::wstring::npos)
    {
        nCount++;
        nLocate++; // 移动到下一个字符继续查找
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

    // 获取文件大小
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    // 确定缓冲区大小
    maxrlen = std::min<size_t>(maxrlen, fileSize);
    std::vector<char> buffer(maxrlen);
    file.read(buffer.data(), maxrlen);
    size_t bytesRead = static_cast<size_t>(file.gcount());
    file.close();

    // 检查BOM（字节顺序标记）
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
    // 如果没有BOM，尝试检测UTF-8（最多检测100个UTF-8字符）
    if (encoding == 0)
    {
        bool isUtf8 = true;
        size_t utf8CharsFound = 0;
        for (size_t i = 0; i < bytesRead && utf8CharsFound < 100; ++i)
        {
            if ((buffer[i] & 0x80) != 0) // 检查高位是否为1
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

    // 根据编码转换内容，跳过BOM
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

#pragma comment(lib, "version.lib")
bool GetFileVerStrW(std::wstring& fnStr, std::wstring& verStr)
{
    wchar_t filePath[MAX_PATH];
    DWORD dwSize = GetModuleFileName(nullptr, filePath, MAX_PATH);
    if (dwSize == 0) return false;

    // 获取不带路径和扩展名的文件名
    std::wstring fullFileName(filePath);
    size_t pos = fullFileName.find_last_of(L"\\/");
    fnStr = (pos == std::wstring::npos) ? fullFileName : fullFileName.substr(pos + 1);
    pos = fnStr.find_last_of(L'.');
    if (pos != std::wstring::npos)
        fnStr = fnStr.substr(0, pos);

    // 获取版本信息
    DWORD dwHandle = NULL;
    DWORD dwFileSize = GetFileVersionInfoSize(filePath, &dwHandle);
    if (dwFileSize == 0) return false;
    BYTE* versionData = new BYTE[dwFileSize];
    if (GetFileVersionInfo(filePath, dwHandle, dwFileSize, versionData))
    {
        VS_FIXEDFILEINFO* fileInfo = nullptr;
        UINT size = 0;
        if (VerQueryValue(versionData, L"\\", (LPVOID*)&fileInfo, &size))
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
            }
        }
    }
    else
    {
        delete[] versionData;
        return false;
    }
    delete[] versionData;
    return true;
}
