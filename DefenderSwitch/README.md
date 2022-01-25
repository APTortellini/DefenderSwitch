# DefenderSwitch
### Stop Windows Defender using the Win32 API

DefenderSwitch uses the standard Windows.h header and the WIL library. To install WIL use vcpkg:
```
C:\vcpkg> .\vcpkg.exe install wil:x86-windows
C:\vcpkg> .\vcpkg.exe install wil:x64-windows
```

Usage
```
C:\Users\last> .\DefenderSwitch.exe -off
C:\Users\last> .\DefenderSwitch.exe -on
```