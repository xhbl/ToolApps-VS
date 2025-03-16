#pragma once

class CBtUtil
{
public:
    /*
    -------------------------------------------------------------------
    Description:
    Creates the actual 'lnk' file (assumes COM has been initialized).

    Parameters:
    pszTargetfile    - File name of the link's target.
    pszTargetargs    - Command line arguments passed to link's target.
    pszLinkfile      - File name of the actual link file being created.
    pszDescription   - Description of the linked item.
    iShowmode        - ShowWindow() constant for the link's target.
    pszCurdir        - Working directory of the active link.
    pszIconfile      - File name of the icon file used for the link.
    iIconindex       - Index of the icon in the icon file.

    Returns:
    HRESULT value >= 0 for success, < 0 for failure.
    --------------------------------------------------------------------
    */
    static HRESULT CreateShortcut(LPCTSTR pszTargetfile, LPCTSTR pszTargetargs,
        LPCTSTR pszLinkfile, LPCTSTR pszDescription,
        int iShowmode, LPCTSTR pszCurdir,
        LPCTSTR pszIconfile, int iIconindex);

    static LPCTSTR GetDesktopPath();
    static LPCTSTR GetAppdataPath();
    static BOOL IsRunElevated();
    static LPCTSTR GetCmdParas();
    static LPCTSTR GetVerString();
};
