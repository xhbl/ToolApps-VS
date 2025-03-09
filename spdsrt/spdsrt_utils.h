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

// 设置标准输出宽字符模式
void SetStdoutModeW(bool bSet);

// 替换字符串中的内容
void StrReplaceW(std::wstring& str, const std::wstring& from, const std::wstring& to);

// 格式化字符串，支持变长参数
std::wstring StrFormatW(const wchar_t* fmt, ...);

// 格式化字符串，支持变长参数
std::wstring StrFormatW(const std::wstring fmt, ...);

// 大小写不敏感的字串查找
size_t StriFindW(const std::wstring& str, const std::wstring& substr, size_t start_pos = 0);

// 去除字符串的前后空白或指定字符
void StrTrimW(std::wstring& str, const wchar_t* tgt = nullptr, int mode = 0);

// 去除字符串左侧或右侧的指定字符
void StrTrimW(std::wstring& str, wchar_t chtgt, int mode = 0);

// 计算字符串中指定字符的出现次数
int NumOfChars(const std::wstring& strTgt, wchar_t chFN);

// 将宽字符字符串转换为Ansi字节串
void WStrToAnsi(char* pcDst, const wchar_t* pcSrc);

// 将宽字符字符串转换为UTF-8字符串
std::string WStrToUtf8(const wchar_t* pcSrc);

// 换行符统一成CRLF
std::wstring NormalizeToCRLF(const std::wstring& input);

// 从字符串中读取一行
size_t ReadOneLine(const std::wstring& strText, std::wstring& strLine, size_t pos);

// 读取文件内容到字符串，支持编码检测
int ReadTextFileW(const wchar_t* filename, std::wstring& output, size_t maxrlen);

// 获得运行程序的名称和版本信息
bool GetFileVerStrW(std::wstring& fnStr, std::wstring& verStr);