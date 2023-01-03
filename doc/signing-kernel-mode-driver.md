It is heard of that signing kernel mode drivers on Windows 10 and above is becoming increasingly hard. In the good old days, a kernel mode driver does not require signing to be loaded by a system like 32-bit Windows 7, 32/64-bit XP and 32/64-bit Vista.

So I decided to use Windows 7/XP first, focus on functionality and leave the signing problem behind. The first option is XP x86 version, to test the newer Linux kernel.

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
 

# Kernel mode code signing policy

https://learn.microsoft.com/en-us/windows-hardware/drivers/install/kernel-mode-code-signing-policy--windows-vista-and-later-

