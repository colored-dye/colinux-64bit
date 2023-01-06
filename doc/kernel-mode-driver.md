# Kernel Mode Driver & Debugging

## Test machine setup

It is heard of that signing kernel mode drivers on Windows 10 and above is becoming increasingly hard. In the "good old days", a kernel mode driver does not require signing to be loaded by 32-bit systems like Windows 7, XP and Vista.

So I decided to use Windows 7/XP first, focus on functionality and leave the signing problem behind. At the first stage, I choose Windows 7 x86 SP1, to test if the newer Linux kernel has been patched the right way.

Then Windows 7 x64 would be used for testing, to see if the 64-bit version of coLinux works correctly.

At last, Windows 10/11 would be tested as these are the most up-to-date Windows systems under official maintenance.


## Kernel mode driver test

In `test/` directory, which includes a minimal driver example, and a rather complete ioctl driver example with a loader, a user-space app and a kernel driver.

`drvtest' credit to "http://support.fccps.cz/download/adv/frr/win32_ddk_mingw/win32_ddk_mingw.html", with minor modification.

To view `DbgPrint()` output, remember to enable "Capture kernel" in DbgView, or use WinDBG.

### Visual Studio example

Workable example: [SIoctl](https://github.com/microsoft/Windows-driver-samples/tree/main/general/ioctl). Remember to comfigure platform and static linking. Can be debugged through WinDBG. This example can be installed without associating to a device or an *.INF file, and managed through SCManager.

## Kernel mode code signing policy

https://learn.microsoft.com/en-us/windows-hardware/drivers/install/kernel-mode-code-signing-policy--windows-vista-and-later-

