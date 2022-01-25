# DefenderSwitch
#### Stop Windows Defender using the Win32 API

DefenderSwitch uses the standard Windows.h header and the WIL library. To install WIL use vcpkg:
```
C:\vcpkg> .\vcpkg.exe install wil:x86-windows
C:\vcpkg> .\vcpkg.exe install wil:x64-windows
```

### Usage
```
C:\Users\last> .\DefenderSwitch.exe -off
C:\Users\last> .\DefenderSwitch.exe -on
```

### Opsec considerations  
This tool as it is written __is definetely not OPSEC safe__. Making it so is left as an exercise to the user.