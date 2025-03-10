#pragma once

#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <memory>
#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <cwctype>
#include <unordered_set>
#include <tuple>

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

// Count occurrences of a specified character in a string
int NumOfChars(const std::wstring& strTgt, wchar_t chFN);

// Convert wide-character string to ANSI byte string
void WStrToAnsi(char* pcDst, const wchar_t* pcSrc);

// Convert wide-character string to UTF-8 string
std::string WStrToUtf8(const wchar_t* pcSrc);

// Normalize line breaks to CRLF
std::wstring NormalizeToCRLF(const std::wstring& input);

// Read one line from a string
size_t ReadOneLine(const std::wstring& strText, std::wstring& strLine, size_t pos);

// Read file content with encoding detection
int ReadTextFileW(const wchar_t* filename, std::wstring& output, size_t maxrlen);

// Get running program's name and version information
bool GetFileVerStrW(std::wstring& fnStr, std::wstring& verStr);
