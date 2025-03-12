# About utouch

**File and Directory Touch Utility with Default UTC Standard**  

[Download | 下载](../Release/utouch.exe?raw=true)

---

## Introduction | 简介

This command-line utility is a versatile tool for modifying file and directory timestamps, with a default adherence to the UTC time standard. Designed for flexibility, it supports precise adjustments to file creation, access, and write times, as well as directory time synchronization. Key features include:

​- **Timestamp Modification: Customize timestamps for files/directories using specific dates (/D), times (/T), or reference files (/F). Users can target write (/W), access (/A), or creation (/C) times individually or in combination, with options to retain original attributes (/K) or use local time (/L).
​- **Recursive Processing: Apply changes recursively (/R) across directories and subdirectories.
​- **Directory Synchronization: Sync directory write times to match the latest timestamp of contained files recursively (/SD), ideal for maintaining consistent backup or project folder metadata.

---

## Usage | 使用方法

Run the program in the command line terminal. | 请在命令行终端里执行本程序。

```
utouch [/W] [/A] [/C] [/S] [/L] [/K] [/D date] [/T time] [/F file] [/R] [/V] files|directory
utouch /SD [/V] directory
```

---

## Options | 参数选项

| Options      | Description                                                                 |
|--------------|-----------------------------------------------------------------------------|
| `/W`         | Change only the write time                                                 |
| `/A`         | Change only the access time                                                |
| `/C`         | Change only the creation time                                              |
| `/S`         | Process directories (default: only files are touched)                      |
| `/L`         | Use local time instead of UTC                                              |
| `/K`         | Keep original date/time if `/D` or `/T` is not specified; else use current |
| `/D date`    | Set date in `mm-dd-yyyy` or `yyyy-mm-dd` format                            |
| `/T time`    | Set time in `hh:mm:ss` (24-hour format)                                    |
| `/F file`    | Use timestamp from a specified reference file                              |
| `/R`         | Recursively process all contents under a directory                         |
| `/V`         | Verbose output with timestamp change details                               |
| `/SD`        | Sync directory write time to latest contained file (recursive)             |
| `files`      | List of files (wildcards supported) to modify                              |
| `directory`  | Target directory path (used with `/R` or `/SD`)                            |

---

## License
This project is licensed under [GPL 3.0 License](../LICENSE.txt). All tools in this repository are free to use, modify, and distribute under the terms of this license.

---

## Contact
**E-mail**: [Send Email](mailto:newxhbl@hotmail.com?subject=[ToolApps-VS]%20Inquiry)  
**Issues**: [Open Issue](../../issues)  
