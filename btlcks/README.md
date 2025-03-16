# About btlcks

**BitLocker Drive Lock/Unlock Switcher**  

[Download](../Release/btlcks.exe?raw=true)

---

## Introduction

This simple utility addresses a security need in multi-user Windows environments. The tool was developed to prevent unintended access when BitLocker-encrypted drives automatically unlock at system startup - a common scenario where multiple users share a computer. By keeping drives securely locked by default, it ensures only authorized users (like the primary account holder) can manually unlock sensitive data through controlled access. It simplifies secure drive management by enabling:

- One-click toggling of drive lock/unlock status via desktop shortcuts.

This tool offers more flexible protection strategies for personal encrypted drives.

---

## Usage

Execute the program via command-line interface to initialize required security credentials and generate a desktop shortcut. This enables simplified one-click drive access management.

```
btlcks DriveLetter [Options]
```
DriveLetter: Specifies the drive letter (A-Z)

---

## Options

| Options      | Description                                                                 |
|--------------|-----------------------------------------------------------------------------|
| `-ck pwd rkey`| Creates a security credential from the recovery key encryped by a password. `pwd`: password used to encrypt, `rkey`: BitLocker recovery key string|
| `-dk pwd`     | Displays the recovery key from the security credential. `pwd`: password used to decrypt, must be same as used in `-ck` option|
| `-cs`        | Creates a desktop shortcut for the specified drive |
| `-lk`        | Locks the drive                                                            |
| `-uk`        | Unlocks the drive                                                          |
| `(No option)`| Toggles the drive's lock/unlock status                                     |

---

## License
This project is licensed under [GPL 3.0 License](../LICENSE.txt). All tools in this repository are free to use, modify, and distribute under the terms of this license.

---

## Contact
**E-mail**: [Send Email](mailto:newxhbl@hotmail.com?subject=[ToolApps-VS]%20Inquiry)  
**Issues**: [Open Issue](../../../issues)  
