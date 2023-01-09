# Development setup

Use **OLD** setup. Visual Studio sucks.

Imaginary setup:

- Build (Debugger): Windows 10 (for kernel driver and user-end app) with WSL2 (Linux kernel).

- Test (Debuggee): Windows (7 32/64-bit, 10 64-bit) VM on another laptop.

Old setup:

- Build: Linux.

- Debugger: Windows on VirtualBox.

- Test: Windows on VirtualBox.

2023/01/09: Use clang to generate PDB debug symbols!

Dependencies: LLVM-MinGW(https://github.com/mstorsjo/llvm-mingw)

Problem: Only works for user programs. Failed to produce correct driver.


## System setup resources

### Test machine systems and utilities

- Windows 7 64-bit:
    "https://download.microsoft.com/download/0/6/3/06365375-C346-4D65-87C7-EE41F55F736B/7601.24214.180801-1700.win7sp1_ldr_escrow_CLIENT_PROFESSIONAL_x64FRE_en-us.iso".

    This is obtained from "https://www.heidoc.net/joomla/technology-science/microsoft/67-microsoft-windows-and-office-iso-download-tool".

    Possibly need to install vcredist packages, from 2005, 2008, 2010, 2012, 2013, 2015 to 2022.

- Windows 7 32-bit:
    "https://archive.org/download/win-7-pro-32-64-iso/32-bit/GSP1RMCPRXFRER_EN_DVD.ISO"

    Product key: FJH38-9YYTR-3RHFDJ-KSFDH-PPTR5 (Credit to @Bill_Gates914 in the link above).

    Windows has issued a security update demanding that drivers be signed with SHA-2 algorithm. But the Windows 7 shown above does not include this update patch. So VMware tools could not be installed. The fix(From: https://vmexplorer.com/2021/11/16/tips-on-installing-windows-7-x32-sp1-on-workstation-16/) is to install the patch inside the VM: https://www.catalog.update.microsoft.com/Search.aspx?q=4474419, and install VMware tools after.

- Windows XP 64-bit:
    "https://archive.org/details/windowsxpprox64english", choose "Windows XP Home x64 With SP2 Vl English.iso".
    
    Product Key: FVMK4-6DD4B-26MB4-74JB2-R4XWM (From: "https://georgia.forumotion.net/t12-microsoft-windows-xp-professional-x64-edition-cd-key").

    Network adapter driver: https://drivers.softpedia.com/get/NETWORK-CARD/INTEL/Intel-Network-Adapter-Driver-182-for-XP64.shtml (On VirtualBox, use the default Intel Pro/1000 network adapter; VMWare chooses for you automatically)

- Windows XP 32-bit:
    "https://archive.org/download/WinXPProSP3x86/en_windows_xp_professional_with_service_pack_3_x86_cd_vl_x14-73974.iso"

    Product key: CCMWP-99T99-KCY96-FGKBW-F9WJT (From: https://archive.org/details/WinXPProSP3x86 with credit to @WNEWBERRYtcs)

    Network adapter driver: https://drivers.softpedia.com/get/NETWORK-CARD/INTEL/Intel-Network-Adapter-Driver-183-for-XP-Server-2003.shtml (On VirtualBox, use the default Intel Pro/1000 network adapter; VMWare chooses for you automatically)

    Windows SDK for Debugging tools: https://www.microsoft.com/download/details.aspx?id=8279

    Windows SDK requires .NET 2.0 x86: https://www.microsoft.com/en-US/Download/confirmation.aspx?id=16614

### Visual Studio and WDK

Possible combination: Visual Studio 2012 Ultimate + WDK 8.0 (Worked fine for `sioctl')

1. Visual Studio, SDK and WDK previous editions::
    https://learn.microsoft.com/en-us/windows-hardware/drivers/other-wdk-downloads

2. WDK Repository: https://networchestration.wordpress.com/2017/12/10/wdk-download/

3. Visual Studio 2012 Ultimate & Product key: https://downloaddevtools.com/en/product/22/free-download-microsoft-visual-studio-ultimate-2012

## Debugging

DebugView: https://download.sysinternals.com/files/DebugView.zip

WinObj: https://download.sysinternals.com/files/WinObj.zip

### Debugging setup

https://blog.csdn.net/xiayuxuan91/article/details/47399551

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

### WinDBG usage

First select "Kernel Debugging", then load symbol path and source path.

To set break points, open a source file, and press F9 to set a break point at cursor line. Kernel debugging supports at most 32 breakpoints.

Another idea: insert "int 3" into code to set hard-coded breakpoints. (Works in `dpp-example')

