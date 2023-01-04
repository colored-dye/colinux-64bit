# Kernel Mode Driver & Debugging

## Virtual machine setup

It is heard of that signing kernel mode drivers on Windows 10 and above is becoming increasingly hard. In the "good old days", a kernel mode driver does not require signing to be loaded by 32-bit systems like Windows 7, XP and Vista.

So I decided to use Windows 7/XP first, focus on functionality and leave the signing problem behind. At the first stage, I choose Windows XP x86 SP3, to test if the newer Linux kernel has been patched the right way.

Then Windows XP x64 SP2 would be used for testing, to see if the 64-bit version of coLinux works correctly.

At the third stage, Windows 7 would be used for driver-related issues.

At last, Windows 10/11 would be tested as these are the most up-to-date Windows systems under official maintenance.

- Windows 7 64-bit:
    "https://download.microsoft.com/download/0/6/3/06365375-C346-4D65-87C7-EE41F55F736B/7601.24214.180801-1700.win7sp1_ldr_escrow_CLIENT_PROFESSIONAL_x64FRE_en-us.iso".

    This is obtained from "https://www.heidoc.net/joomla/technology-science/microsoft/67-microsoft-windows-and-office-iso-download-tool".

    Possibly need to install vcredist packages, from 2005, 2008, 2010, 2012, 2013, 2015 to 2022.

- Windows XP 64-bit:
    "https://archive.org/details/windowsxpprox64english", choose "Windows XP Home x64 With SP2 Vl English.iso".
    
    Product Key: FVMK4-6DD4B-26MB4-74JB2-R4XWM (From: "https://georgia.forumotion.net/t12-microsoft-windows-xp-professional-x64-edition-cd-key").

    Network adapter driver: https://drivers.softpedia.com/get/NETWORK-CARD/INTEL/Intel-Network-Adapter-Driver-182-for-XP64.shtml (On VirtualBox, use the default Intel Pro/1000 network adapter; VMWare chooses for you automatically)

    DebugView: https://download.sysinternals.com/files/DebugView.zip

- Windows XP 32-bit:
    "https://archive.org/download/WinXPProSP3x86/en_windows_xp_professional_with_service_pack_3_x86_cd_vl_x14-73974.iso"

    Product key: CCMWP-99T99-KCY96-FGKBW-F9WJT (From: https://archive.org/details/WinXPProSP3x86 with credit to @WNEWBERRYtcs)

    Network adapter driver: https://drivers.softpedia.com/get/NETWORK-CARD/INTEL/Intel-Network-Adapter-Driver-183-for-XP-Server-2003.shtml (On VirtualBox, use the default Intel Pro/1000 network adapter; VMWare chooses for you automatically)

    Windows SDK for Debugging tools: https://www.microsoft.com/download/details.aspx?id=8279

    Windows SDK requires .NET 2.0 x86: https://www.microsoft.com/en-US/Download/confirmation.aspx?id=16614

## Debugging

    To enable kernel debugging: append "/debug /debugport=com1 /baudrate=115200" to C:\boot.ini. Then go to VirtualBox settings, and add a serial port with port mode "Host Pipe". Then create a second VM and connect through a serial port via the host pipe.

    Debugger VM and debuggee VM setting on a Linux host: (Credit to: https://www.matteomalvica.com/blog/2018/11/16/windows-xp-kernel-debugging-on-vmware-fusion/)

    - Debugger:
        serial0.present = "TRUE"
        serial0.fileType = "pipe"
        serial0.yieldOnMsrRead = "TRUE"
        serial0.startConnected = "TRUE"
        serial0.fileName = "/tmp/dev_com"
        serial0.pipe.endPoint = "client"

    - Debuggee:
        serial0.present = "TRUE"
        serial0.fileType = "pipe"
        serial0.yieldOnMsrRead = "TRUE"
        serial0.startConnected = "TRUE"
        serial0.fileName = "/tmp/dev_com"
        serial0.pipe.endPoint = "server"

    First, connect to debug port via WinDBG in debugger VM, then boot debuggee with debug boot entry.

    If on a Windows host, replace serial0.fileName with `\\.\pipe\com_1`.
 

## Kernel mode driver test

In `test/` directory, which includes a minimal driver example, and a rather complete ioctl driver example with a loader, a user-space app and a kernel driver.

`drvtest' credit to "http://support.fccps.cz/download/adv/frr/win32_ddk_mingw/win32_ddk_mingw.html", with minor modification.


## Kernel mode code signing policy

https://learn.microsoft.com/en-us/windows-hardware/drivers/install/kernel-mode-code-signing-policy--windows-vista-and-later-

