#pragma once

#include "spdsrt_utils.h"

int DoSpdsrt(int argc, wchar_t* argv[]);

struct stMeta
{
    int    iPlayResXDft;
    int    iPlayResYDft;
    int    iPlayResXSrc;
    int    iPlayResYSrc;

    const wchar_t* szFontCh;
    int    iFontCh;
    int    iFontChCal;
    const wchar_t* szFontEn;
    int    iFontEn;
    int    iFontEnCal;
    const wchar_t* szFontDft;
    int    iFontDft;
    int    iFontDftCal;

    int    iMarginL;
    int    iMarginR;
    int    iMarginV;
    int    iMarginLCal;
    int    iMarginRCal;
    int    iMarginVCal;

    int    iSlvCh;
    int    iSlvChCal;
    int    iSlvEn;
    int    iSlvEnCal;

    int    iEncodeCh;
    int    iEncodeEn;

    const wchar_t* szStyleNameDft;
    const wchar_t* szStyleNameDftAlt;
    const wchar_t* szStyleFmt;
    wchar_t  szStyleDftCh[256];
    wchar_t  szStyleDftEn[256];
    wchar_t  szStyleCalCh[256];
    wchar_t  szStyleCalEn[256];

    const wchar_t* szAssHdrFmt;
    wchar_t  szAssHdrDftCh[4096];
    wchar_t  szAssHdrDftEn[4096];
    wchar_t  szAssHdrSrcCh[4096];
    wchar_t  szAssHdrSrcEn[4096];
    wchar_t  szAssHdrSrcOrg[4096];
};

class CSrtNode
{
public:
    int nLine;
    std::wstring strTime;
    std::vector<std::wstring> astrOrg;
    std::vector<std::wstring> astrCh;
    std::vector<std::wstring> astrEn;
    std::vector<std::wstring> astrAssCh;
    std::vector<std::wstring> astrAssEn;
    int iAssKeep;
    std::wstring strAssFmt;
    std::wstring strAssTxt;
    std::wstring strAssOrg;
    bool bAssUnknown;
    bool bSrtDup;

public:
    CSrtNode() : nLine(0), bAssUnknown(false), bSrtDup(false), iAssKeep(0) {}
};

enum osfMode
{
    or_srt,  // 原始字幕
    ec_srt,  // 英+中
    ce_srt,  // 中+英
    en_srt,  // 仅英文
    ch_srt,  // 仅中文
    ec_ass,  // ASS格式英中
    ce_ass,  // ASS格式中英
    en_ass,  // ASS仅英文
    ch_ass   // ASS仅中文
};
