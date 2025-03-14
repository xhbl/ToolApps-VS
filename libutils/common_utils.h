#pragma once

// Windows headers
#include <windows.h>
// C runtime headers
#include <io.h>
#include <fcntl.h>
// C++ standard headers
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <cwctype>
#include <array>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <tuple>
#include <regex>

// Set standard output to wide character mode
void SetStdoutModeW(bool bSet);

// Print formatted string to console
int ConPrintfW(const wchar_t* fmt, ...);

// Replace substrings within a string
void StrReplaceW(std::wstring& str, const std::wstring& from, const std::wstring& to);

// Format string with variable arguments
std::wstring StrFormatW(const wchar_t* fmt, ...);

// Format string with variable arguments
std::wstring StrFormatW(const std::wstring fmt, ...);

// Case-insensitive substring search
size_t StriFindW(const std::wstring& str, const std::wstring& substr, size_t start_pos = 0);

// Trim leading/trailing whitespace or specified characters
void StrTrimW(std::wstring& str, const wchar_t* tgt = nullptr, int mode = 0);

// Trim leading or trailing specified character
void StrTrimW(std::wstring& str, wchar_t chtgt, int mode = 0);

// Convert wide-character string to uppercase
std::wstring StrToUpperW(const std::wstring& str);

// Convert wide-character string to lowercase
std::wstring StrToLowerW(const std::wstring& str);

// Count occurrences of a specified character in a string
int NumOfChars(const std::wstring& strTgt, wchar_t chFN);

// Convert wide-character string to ANSI byte string
void WStrToAnsi(char* pcDst, const wchar_t* pcSrc);

// Convert wide-character string to UTF-8 string
std::string WStrToUtf8(const wchar_t* pcSrc);

// Normalize line breaks to CRLF
std::wstring NormalizeToCRLF(const std::wstring& input);

// Validate date string
bool ValidateDateStrW(const std::wstring& date_str, int* pyy = nullptr, int* pmm = nullptr, int* pdd = nullptr);

// Validate time string
bool ValidateTimeStrW(const std::wstring& time_str, int* phh = nullptr, int* pmm = nullptr, int* pss = nullptr, int* pSSS = nullptr);

// Read one line from a string
size_t ReadOneLine(const std::wstring& strText, std::wstring& strLine, size_t pos);

// Read file content with encoding detection
int ReadTextFileW(const wchar_t* filename, std::wstring& output, size_t maxrlen);

// Check if a path exists and its type
enum enPEType
{
    PET_NONE,       // path not exist
    PET_FILE,       // path is a file
    PET_DIR,        // path is a directory
    PET_WCEXIST,    // path with wildcard exists a match
    PET_WCDIROK,    // path with wildcard directory part exists
    PET_WCDIRNO,    // path with wildcard directory part not exist
    PET_WCDIRNA     // path with wildcard has no directory part
};
enum enPEType PathExistType(const std::wstring& path);

// Check if a path is a exist file
bool PathExistFile(const std::wstring& path);

// Check if a path is a exist directory
bool PathExistDir(const std::wstring& path);

// Normalize path string
void PathNormalizeW(std::wstring& path, bool keeprootslash, bool normalizeslash = false);

// Append file name to path
std::wstring PathAppendFileW(const std::wstring& path, const std::wstring& file);

// Get running program's name and version information
bool GetFileVerStrW(std::wstring& fnStr, std::wstring& verStr);
