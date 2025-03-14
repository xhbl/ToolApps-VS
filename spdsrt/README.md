# About spdsrt

**Standard Presentation Designer for Chinese-English Bilingual Subtitles**  
**中英文双语字幕标准化呈现设计器**

![SPDSRT示例图](spdsrt_samp.jpg)

[Download | 下载](../Release/spdsrt.exe?raw=true)

---

## Introduction | 简介

This is a CLI tool designed to standardize the style formatting of Chinese-English bilingual subtitle files, generating multiple subtitle files with different language arrangement combinations.

这是一款命令行工具，设计用于标准化中英文双语字幕文件的样式格式，能输出多个不同语言顺序组合的字幕文件。

---

## Usage | 使用方法

Run the program in the command line terminal. | 请在命令行终端里执行本程序。

```
spdsrt input.srt [-b] [-u] [-s] [-k] [-t]
spdsrt input.ass [-b] [-u] [-s] [-k] [-d] [-f] [-a] [-t] [-z<n>] [-c<n>]
```

- **Input File**: A `.srt` or `.ass` subtitle file containing both Chinese and English lines.
- **输入文件**：一个包含中英文双语内容的`.srt`或`.ass`文件
- **Output Files**: Nine formatted output subtitle files include the following:
- **输出文件**：九个经过格式化输出的字幕文件包括如下：

    [.or.srt] -- Original Text | 原始序列文本

    [.ce.srt/ass] -- Chinese top/English bottom | 中上英下双语

    [.ec.srt/ass] -- English top/Chinese bottom | 英上中下双语

    [.ch.srt/ass] -- Chinese-only | 纯中文

    [.en.srt/ass] -- English-only | 纯英文

---

## Options | 参数选项

| Options | Description | 中文描述 |
|---------|------------|--------------|
| `-b` | Add round Brackets to Chinese-only single line | 给单行的纯中文行外加括号 |
| `-u` | Upper-align Chinese-only line(s) in output ASS | 纯中文行在输出的ASS文件中上边对齐 |
| `-s` | Merge multiple lines into a Single line, Chinese/English respectively | 合并多行为单行，中文/英文分别处理 |
| `-k` | Keep empty line(s) in the output | 输出文件中保留原有的空行 |
| `-d` | Discard all input ASS styles and use built-in styles for the output (not used with -f -a) | 忽略所有源ASS样式，使用内置样式输出（不要和-f -a同时使用） |
| `-f` | Use built-in Font style to override input ASS font (built-in Chinese Font: `SIMHEI`, English Font: `Tahoma`) | 使用内置字体样式覆盖源ASS内字体 (内置中文字体：`黑体`, 英文字体：`Tahoma`) |
| `-a` | Skip Animated styles from input ASS in the output | 忽略源ASS文件中的动画样式 |
| `-t` | Sort output by subtitle Timestamp | 根据字幕时间戳排序后输出 |
| `-z<n>` | SiZe of the font: z1-small, z2-medium (default), z3-large | 字体大小： z1-小 z2-中(默认) z3-大 |
| `-c<n>` | Color of English font, c1-yellow(default) c2-orange (Chinese color is white) | 英文字体颜色： c1-黄色(默认) c2-橙色 (中文字体为白色) |

---

## Auxiliary Functions | 辅助功能

```
spdsrt input.ass -res<XXX>x<YYY>
spdsrt input.ass -169
spdsrt input.ass|srt -mf<FR>
```

| Options | Description | 中文描述 |
|---------|------------|--------------|
| `-res<Wx><H>` | Resize ASS play resolution, `<Wx>`-width, `<H>`-height. Output file name suffixed with `.res`, Example: -res512x288, -res288, -res512x | 调整 ASS 基准分辨率，输出文件名后缀 `.res` |
| `-169` | Map widescreen(2.35:1) coordinates to 16:9 display. Output file name suffixed with `.169` | 映射宽屏(2.35:1)坐标到16:9模式，输出文件名后缀 `.169` |
| `-mf<FR>` | Get Maximum timestamp Frames by rate, `<FR>`-frame rate. Example: `-mf23.976`, `-mf25` | 输出字幕最大时间码对应的帧数 |

---

## License | 许可协议
This project is licensed under [GPL 3.0 License](../LICENSE.txt). All tools in this repository are free to use, modify, and distribute under the terms of this license.

本项目遵循 [GPL 3.0 协议](../LICENSE.txt) 授权。本仓库中所有工具均可在该协议条款下自由使用、修改和分发。

---

## Contact | 联系我们
**E-mail | 邮箱**: [Send Email | 发送邮件](mailto:newxhbl@hotmail.com?subject=[ToolApps-VS]%20Inquiry)  
**Issues | 问题**: [Open Issue | 提交问题](../../issues)  
