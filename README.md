## Studying primitive colinux

Here I choose the latest devel-64bit version of the original colinux source code. But according to README64 in this repository, neither could the devel-64bit version be successfully compiled fully, nor is it completely transformed to the 64-bit version. So there's still quite a lot of work to do.

```
.
├── bin
│   ├── autotrace.py
│   ├── build-all.sh
│   ├── build-colinux-libs.sh
│   ├── build-colinux.sh
│   ├── build-common.sh
│   ├── build-cross.sh
│   ├── build-kernel.sh
│   ├── comake
│   │   ├── cdeps.py
│   │   ├── defaults.py
│   │   ├── __init__.py
│   │   ├── lib.py
│   │   ├── report.py
│   │   ├── settings.py
│   │   ├── target.py
│   │   └── tools.py
│   ├── make.py
│   ├── mindump-repaint-labels.sh
│   ├── premaid.sh
│   ├── sample.user-build.cfg
│   └── tracewrapper.py
├── build.comake.py
├── conf
│   ├── busybox-1.00-rc3-config
│   ├── example.conf
│   ├── initrd-linuxrc
│   ├── linux-2.6.25.20-config
│   ├── linux-2.6.26.8-config
│   ├── linux-2.6.33.5-config
│   ├── README
│   └── wxWidgets.conf
├── configure
├── COPYING
├── CREDITS
├── doc
│   ├── building
│   ├── cofs
│   ├── colinux-daemon
│   └── debugging
├── Makefile
├── Makefile.linux
├── Makefile.winnt
├── NEWS
├── patch
├── README64
├── RUNNING
├── src
│   ├── build.comake.py
│   ├── colinux
│   │   ├── arch
│   │   │   ├── build.comake.py
│   │   │   ├── i386
│   │   │   │   ├── antinx.c
│   │   │   │   ├── antinx.h
│   │   │   │   ├── apic.c
│   │   │   │   ├── apic.h
│   │   │   │   ├── build.comake.py
│   │   │   │   ├── cpuid.c
│   │   │   │   ├── cpuid.h
│   │   │   │   ├── defs.h
│   │   │   │   ├── interrupt.c
│   │   │   │   ├── manager.c
│   │   │   │   ├── manager.h
│   │   │   │   ├── mmu.h
│   │   │   │   ├── passage.c
│   │   │   │   ├── utils.c
│   │   │   │   └── utils.h
│   │   │   ├── interrupt.h
│   │   │   ├── manager.h
│   │   │   ├── mmu.h
│   │   │   ├── passage.h
│   │   │   └── x86_64
│   │   │       ├── antinx.c
│   │   │       ├── antinx.h
│   │   │       ├── apic.c
│   │   │       ├── apic.h
│   │   │       ├── build.comake.py
│   │   │       ├── cpuid.c
│   │   │       ├── cpuid.h
│   │   │       ├── defs.h
│   │   │       ├── interrupt.c
│   │   │       ├── manager.c
│   │   │       ├── manager.h
│   │   │       ├── mmu.h
│   │   │       ├── passage.c
│   │   │       ├── utils.c
│   │   │       └── utils.h
│   │   ├── build.comake.py
│   │   ├── common
│   │   │   ├── build.comake.py
│   │   │   ├── common64.h
│   │   │   ├── common_base.h
│   │   │   ├── common.h
│   │   │   ├── config.h
│   │   │   ├── console.c
│   │   │   ├── console.h
│   │   │   ├── debug.c
│   │   │   ├── debug.h
│   │   │   ├── errors.c
│   │   │   ├── import.h
│   │   │   ├── ioctl.h
│   │   │   ├── libc.c
│   │   │   ├── libc.h
│   │   │   ├── list.h
│   │   │   ├── messages.c
│   │   │   ├── messages.h
│   │   │   ├── module_repr.c
│   │   │   ├── queue.c
│   │   │   ├── queue.h
│   │   │   ├── snprintf.c
│   │   │   ├── unicode.c
│   │   │   ├── unicode.h
│   │   │   └── version.py
│   │   ├── kernel
│   │   │   ├── block.c
│   │   │   ├── block.h
│   │   │   ├── build.comake.py
│   │   │   ├── debug.c
│   │   │   ├── debug.h
│   │   │   ├── fileblock.c
│   │   │   ├── fileblock.h
│   │   │   ├── filesystem.c
│   │   │   ├── filesystem.h
│   │   │   ├── manager.c
│   │   │   ├── manager.h
│   │   │   ├── monitor.c
│   │   │   ├── monitor.h
│   │   │   ├── pages.c
│   │   │   ├── pages.h
│   │   │   ├── pci.c
│   │   │   ├── pci.h
│   │   │   ├── README
│   │   │   ├── reversedpfns.c
│   │   │   ├── reversedpfns.h
│   │   │   ├── scsi.c
│   │   │   ├── scsi.h
│   │   │   ├── transfer.c
│   │   │   ├── transfer.h
│   │   │   ├── video.c
│   │   │   └── video.h
│   │   ├── os
│   │   │   ├── alloc.h
│   │   │   ├── kernel
│   │   │   │   ├── alloc.h
│   │   │   │   ├── filesystem.h
│   │   │   │   ├── manager.h
│   │   │   │   ├── misc.h
│   │   │   │   ├── monitor.h
│   │   │   │   ├── mutex.h
│   │   │   │   ├── time.h
│   │   │   │   ├── user.h
│   │   │   │   └── wait.h
│   │   │   ├── linux
│   │   │   │   ├── build
│   │   │   │   │   └── build.comake.py
│   │   │   │   ├── ioctl.h
│   │   │   │   ├── kernel
│   │   │   │   │   ├── core.c
│   │   │   │   │   └── module
│   │   │   │   │       ├── alloc.c
│   │   │   │   │       ├── build.comake.py
│   │   │   │   │       ├── colinux.c
│   │   │   │   │       ├── conet.c
│   │   │   │   │       ├── debug.c
│   │   │   │   │       ├── file.c
│   │   │   │   │       ├── fs.c
│   │   │   │   │       ├── linux_inc.h
│   │   │   │   │       ├── main.c
│   │   │   │   │       ├── Makefile
│   │   │   │   │       ├── Makefile.include
│   │   │   │   │       ├── manager.c
│   │   │   │   │       ├── manager.h
│   │   │   │   │       ├── misc.c
│   │   │   │   │       ├── mutex.c
│   │   │   │   │       ├── pages.c
│   │   │   │   │       ├── scsi.c
│   │   │   │   │       ├── time.c
│   │   │   │   │       ├── timer.c
│   │   │   │   │       ├── user.c
│   │   │   │   │       └── wait.c
│   │   │   │   ├── memory.h
│   │   │   │   ├── test
│   │   │   │   │   └── pipe_server.c
│   │   │   │   └── user
│   │   │   │       ├── alloc.c
│   │   │   │       ├── build.comake.py
│   │   │   │       ├── cobdpath.c
│   │   │   │       ├── conet-daemon
│   │   │   │       │   ├── build.comake.py
│   │   │   │       │   ├── daemon.cpp
│   │   │   │       │   ├── daemon.h
│   │   │   │       │   ├── tap.c
│   │   │   │       │   └── tap.h
│   │   │   │       ├── conet-slirp-daemon
│   │   │   │       │   ├── build.comake.py
│   │   │   │       │   └── main.c
│   │   │   │       ├── console-fltk
│   │   │   │       │   ├── build.comake.py
│   │   │   │       │   └── head.cpp
│   │   │   │       ├── coserial-daemon
│   │   │   │       │   ├── build.comake.py
│   │   │   │       │   └── main.c
│   │   │   │       ├── daemon
│   │   │   │       │   ├── build.comake.py
│   │   │   │       │   ├── debug.c
│   │   │   │       │   ├── main.c
│   │   │   │       │   └── registry.c
│   │   │   │       ├── debug
│   │   │   │       │   ├── build.comake.py
│   │   │   │       │   └── main.c
│   │   │   │       ├── exec.c
│   │   │   │       ├── file.c
│   │   │   │       ├── file-unlink.c
│   │   │   │       ├── file-write.c
│   │   │   │       ├── manager.c
│   │   │   │       ├── misc.c
│   │   │   │       ├── process.c
│   │   │   │       ├── reactor.c
│   │   │   │       ├── reactor.h
│   │   │   │       ├── unix.c
│   │   │   │       └── unix.h
│   │   │   ├── README
│   │   │   ├── timer.h
│   │   │   ├── user
│   │   │   │   ├── cobdpath.h
│   │   │   │   ├── config.h
│   │   │   │   ├── daemon.h
│   │   │   │   ├── exec.h
│   │   │   │   ├── file.h
│   │   │   │   ├── manager.h
│   │   │   │   ├── misc.h
│   │   │   │   └── reactor.h
│   │   │   └── winnt
│   │   │       ├── build
│   │   │       │   └── build.comake.py
│   │   │       ├── ioctl.h
│   │   │       ├── kernel
│   │   │       │   ├── alloc.c
│   │   │       │   ├── block.c
│   │   │       │   ├── build.comake.py
│   │   │       │   ├── conet.c
│   │   │       │   ├── conet.h
│   │   │       │   ├── core.c
│   │   │       │   ├── ddk.h
│   │   │       │   ├── driver.h
│   │   │       │   ├── exceptions.h
│   │   │       │   ├── fileio.c
│   │   │       │   ├── fileio.h
│   │   │       │   ├── filesystem.c
│   │   │       │   ├── interface.c
│   │   │       │   ├── lowlevel
│   │   │       │   │   ├── alloc.c
│   │   │       │   │   ├── build.comake.py
│   │   │       │   │   ├── debug.c
│   │   │       │   │   ├── misc.c
│   │   │       │   │   ├── mutex.c
│   │   │       │   │   ├── time.c
│   │   │       │   │   ├── timer.c
│   │   │       │   │   ├── user.c
│   │   │       │   │   └── wait.c
│   │   │       │   ├── manager.c
│   │   │       │   ├── manager.h
│   │   │       │   ├── scsi.c
│   │   │       │   ├── tap-win32
│   │   │       │   │   ├── common.h
│   │   │       │   │   ├── config-win32.h
│   │   │       │   │   ├── constants.h
│   │   │       │   │   ├── dhcp.c
│   │   │       │   │   ├── dhcp.h
│   │   │       │   │   ├── endian.h
│   │   │       │   │   ├── error.c
│   │   │       │   │   ├── error.h
│   │   │       │   │   ├── hexdump.c
│   │   │       │   │   ├── hexdump.h
│   │   │       │   │   ├── i386
│   │   │       │   │   │   ├── OemWin2k.inf
│   │   │       │   │   │   └── tap.cat
│   │   │       │   │   ├── instance.c
│   │   │       │   │   ├── lock.h
│   │   │       │   │   ├── macinfo.c
│   │   │       │   │   ├── macinfo.h
│   │   │       │   │   ├── MAKEFILE
│   │   │       │   │   ├── mem.c
│   │   │       │   │   ├── proto.h
│   │   │       │   │   ├── prototypes.h
│   │   │       │   │   ├── resource.rc
│   │   │       │   │   ├── SOURCES
│   │   │       │   │   ├── tapdrvr.c
│   │   │       │   │   └── types.h
│   │   │       │   ├── time.c
│   │   │       │   └── time.h
│   │   │       ├── memory.h
│   │   │       ├── monitor.h
│   │   │       ├── os.h
│   │   │       └── user
│   │   │           ├── alloc.c
│   │   │           ├── build.comake.py
│   │   │           ├── cobdpath.c
│   │   │           ├── conet-bridged-daemon
│   │   │           │   ├── build.comake.py
│   │   │           │   ├── main.c
│   │   │           │   └── pcap-registry.h
│   │   │           ├── conet-daemon
│   │   │           │   ├── build.comake.py
│   │   │           │   ├── daemon.cpp
│   │   │           │   ├── daemon.h
│   │   │           │   ├── tap-win32.c
│   │   │           │   └── tap-win32.h
│   │   │           ├── conet-ndis-daemon
│   │   │           │   ├── build.comake.py
│   │   │           │   ├── main.c
│   │   │           │   └── pcap-registry.h
│   │   │           ├── conet-slirp-daemon
│   │   │           │   ├── build.comake.py
│   │   │           │   └── main.c
│   │   │           ├── console-fltk
│   │   │           │   ├── build.comake.py
│   │   │           │   └── head.cpp
│   │   │           ├── console-nt
│   │   │           │   ├── build.comake.py
│   │   │           │   ├── console.cpp
│   │   │           │   ├── console.h
│   │   │           │   ├── head.cpp
│   │   │           │   ├── os_console.c
│   │   │           │   ├── os_console.h
│   │   │           │   ├── widget.cpp
│   │   │           │   └── widget.h
│   │   │           ├── coserial-daemon
│   │   │           │   ├── build.comake.py
│   │   │           │   └── main.c
│   │   │           ├── daemon
│   │   │           │   ├── build.comake.py
│   │   │           │   ├── cmdline.c
│   │   │           │   ├── cmdline.h
│   │   │           │   ├── debug.c
│   │   │           │   ├── debug.h
│   │   │           │   ├── driver.c
│   │   │           │   ├── driver.h
│   │   │           │   ├── main.c
│   │   │           │   ├── main.h
│   │   │           │   ├── misc.c
│   │   │           │   ├── misc.h
│   │   │           │   ├── registry.c
│   │   │           │   ├── res
│   │   │           │   │   ├── build.comake.py
│   │   │           │   │   ├── colinux.ico
│   │   │           │   │   ├── colinux.rc
│   │   │           │   │   ├── daemon.rc
│   │   │           │   │   ├── resources_def.sh
│   │   │           │   │   ├── service-message.bin
│   │   │           │   │   ├── service-message.h
│   │   │           │   │   └── service-message.mc
│   │   │           │   ├── service.c
│   │   │           │   └── service.h
│   │   │           ├── debug
│   │   │           │   ├── build.comake.py
│   │   │           │   └── main.c
│   │   │           ├── exec.c
│   │   │           ├── file.c
│   │   │           ├── file-unlink.c
│   │   │           ├── file-write.c
│   │   │           ├── install
│   │   │           │   ├── build.comake.py
│   │   │           │   ├── colinux_def.sh
│   │   │           │   ├── colinux.nsi
│   │   │           │   ├── header.bmp
│   │   │           │   ├── iDl.ini
│   │   │           │   ├── startlogo.bmp
│   │   │           │   ├── WinpcapRedir.ini
│   │   │           │   └── WriteEnvStr.nsh
│   │   │           ├── manager.c
│   │   │           ├── manager.h
│   │   │           ├── misc.c
│   │   │           ├── misc.h
│   │   │           ├── process.c
│   │   │           ├── reactor.c
│   │   │           ├── reactor.h
│   │   │           ├── timer.c
│   │   │           └── udp.c
│   │   ├── user
│   │   │   ├── build.comake.py
│   │   │   ├── cmdline.c
│   │   │   ├── cmdline.h
│   │   │   ├── config.c
│   │   │   ├── config.h
│   │   │   ├── configurator
│   │   │   │   ├── blockdevice.py
│   │   │   │   ├── common.py
│   │   │   │   ├── configurator.py
│   │   │   │   ├── networkdevice.py
│   │   │   │   ├── README
│   │   │   │   └── xmlwrapper.py
│   │   │   ├── console-fltk
│   │   │   │   ├── about.cpp
│   │   │   │   ├── about.h
│   │   │   │   ├── build.comake.py
│   │   │   │   ├── console.cpp
│   │   │   │   ├── console.h
│   │   │   │   ├── fontselect.cpp
│   │   │   │   ├── fontselect.h
│   │   │   │   ├── img
│   │   │   │   │   ├── colinux_logo_3d.xpm
│   │   │   │   │   └── colinux_logo.xpm
│   │   │   │   ├── main.cpp
│   │   │   │   ├── main.h
│   │   │   │   ├── select_monitor.cpp
│   │   │   │   ├── select_monitor.h
│   │   │   │   ├── widget.cpp
│   │   │   │   └── widget.h
│   │   │   ├── console-nt
│   │   │   │   ├── build.comake.py
│   │   │   │   ├── console.cpp
│   │   │   │   ├── console.h
│   │   │   │   ├── main.cpp
│   │   │   │   ├── main.h
│   │   │   │   ├── widget.cpp
│   │   │   │   └── widget.h
│   │   │   ├── console-wx
│   │   │   │   ├── build.comake.py
│   │   │   │   ├── console.h
│   │   │   │   └── main.cpp
│   │   │   ├── context.h
│   │   │   ├── control
│   │   │   │   └── main.c
│   │   │   ├── daemon-base
│   │   │   │   ├── build.comake.py
│   │   │   │   ├── main.cpp
│   │   │   │   └── main.h
│   │   │   ├── daemon.c
│   │   │   ├── daemon.h
│   │   │   ├── debug
│   │   │   │   ├── build.comake.py
│   │   │   │   ├── dump.py
│   │   │   │   ├── main.c
│   │   │   │   ├── main.h
│   │   │   │   └── server.py
│   │   │   ├── debug.c
│   │   │   ├── debug.h
│   │   │   ├── elf_load.c
│   │   │   ├── elf_load.h
│   │   │   ├── macaddress.c
│   │   │   ├── macaddress.h
│   │   │   ├── manager.c
│   │   │   ├── manager.h
│   │   │   ├── monitor.c
│   │   │   ├── monitor-find.c
│   │   │   ├── monitor.h
│   │   │   ├── pidfile.c
│   │   │   ├── reactor.c
│   │   │   ├── reactor.h
│   │   │   ├── README
│   │   │   └── slirp
│   │   │       ├── bootp.c
│   │   │       ├── bootp.h
│   │   │       ├── build.comake.py
│   │   │       ├── cksum.c
│   │   │       ├── co_main.c
│   │   │       ├── co_main.h
│   │   │       ├── config.h
│   │   │       ├── COPYRIGHT
│   │   │       ├── ctl.h
│   │   │       ├── debug.c
│   │   │       ├── debug.h
│   │   │       ├── icmp_var.h
│   │   │       ├── if.c
│   │   │       ├── if.h
│   │   │       ├── ip.h
│   │   │       ├── ip_icmp.c
│   │   │       ├── ip_icmp.h
│   │   │       ├── ip_input.c
│   │   │       ├── ip_output.c
│   │   │       ├── libslirp.h
│   │   │       ├── main.h
│   │   │       ├── mbuf.c
│   │   │       ├── mbuf.h
│   │   │       ├── misc.c
│   │   │       ├── misc.h
│   │   │       ├── sbuf.c
│   │   │       ├── sbuf.h
│   │   │       ├── slirp.c
│   │   │       ├── slirp_config.h
│   │   │       ├── slirp.h
│   │   │       ├── socket.c
│   │   │       ├── socket.h
│   │   │       ├── tcp.h
│   │   │       ├── tcp_input.c
│   │   │       ├── tcpip.h
│   │   │       ├── tcp_output.c
│   │   │       ├── tcp_subr.c
│   │   │       ├── tcp_timer.c
│   │   │       ├── tcp_timer.h
│   │   │       ├── tcp_var.h
│   │   │       ├── tftp.c
│   │   │       ├── tftp.h
│   │   │       ├── udp.c
│   │   │       └── udp.h
│   │   └── VERSION
│   ├── intro
│   └── Makefile
├── THANKS
├── TODO
└── TODO64
```

## Reimplementing colinux

I try not to be boastful, but my plan is not at all easy:

1. Reorganize the entire structure of colinux by employing modern project organizing tools like `xmake`.

    The reason for reimplementing, rather than simply modifying, is that the original structure utilizes Python 2 for extracting symbols for further debugging, which may be better handled with modern debugging tools.

2. The goal of my new colinux is to run on a 64-bit Windows machine (e.g, Windows 10/11), but with either 32-bit or 64-bit kernel. One of the reasons why da-x and so many others did not go further in migrating colinux on 64-bit platforms was that there weren’t effective driver signing techniques back then(according to the dashboard in colinux Wiki). But there are already some mature driver signing tools available now, like driver signature enforcement overrider.

## Tools needed

1.   GCC: https://ftp.gnu.org/pub/gnu/gcc/
1.   MinGW-w64: https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v10.0.0.tar.bz2
2.   WinPCAP
3.   GNU binutils
4.   W32API
5.   FLTK





















