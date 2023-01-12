.
├── ./bin
│   ├── ./bin/autotrace.py
│   ├── ./bin/build-all.sh
│   ├── ./bin/build-common.sh -> mingw-w64/build-common.sh
│   ├── ./bin/build-cross.sh -> mingw-w64/build-cross.sh
│   ├── ./bin/comake
│   │   ├── ./bin/comake/cdeps.py
│   │   ├── ./bin/comake/defaults.py
│   │   ├── ./bin/comake/__init__.py
│   │   ├── ./bin/comake/lib.py
│   │   ├── ./bin/comake/report.py
│   │   ├── ./bin/comake/settings.py
│   │   ├── ./bin/comake/target.py
│   │   └── ./bin/comake/tools.py
│   ├── ./bin/make.py
│   ├── ./bin/mingw32
│   │   ├── ./bin/mingw32/build-common.sh
│   │   ├── ./bin/mingw32/build-cross.sh
│   │   └── ./bin/mingw32/install-gcc4.1.2.sh
│   ├── ./bin/mingw-w64
│   │   ├── ./bin/mingw-w64/build-common.sh
│   │   ├── ./bin/mingw-w64/build-cross.sh
│   │   ├── ./bin/mingw-w64/build-cross.sh.bak
│   │   └── ./bin/mingw-w64/mingw-w64-build.sh
│   ├── ./bin/README.md
│   ├── ./bin/tracewrapper.py
│   └── ./bin/user-build.cfg
├── ./build.comake.py
├── ./compile_flags.txt
├── ./conf
├── ./configure
├── ./doc
│   ├── ./doc/devel.md
│   ├── ./doc/kernel-mode-driver.md
│   ├── ./doc/kmcs_walkthrough.doc
│   ├── ./doc/progress.md
│   ├── ./doc/tree.md
│   └── ./doc/urls.md
├── ./env
├── ./get_patch_file.py
├── ./Makefile
├── ./Makefile.old
├── ./Makefile.winnt.mk
├── ./patch
│   ├── ./patch/audio-2.6.25.diff
│   ├── ./patch/audio-2.6.26.diff
│   ├── ./patch/audio-2.6.33.diff
│   ├── ./patch/audio-core.diff
│   ├── ./patch/base-2.6.25.diff
│   ├── ./patch/base-2.6.26.diff
│   ├── ./patch/base-2.6.33.diff
│   ├── ./patch/cloop-2.6.25.diff
│   ├── ./patch/cloop-2.6.26.diff
│   ├── ./patch/cloop-2.6.33.diff
│   ├── ./patch/cloop-core-2.628.diff
│   ├── ./patch/cloop-core-2.631.diff
│   ├── ./patch/cobd-2.6.25.diff
│   ├── ./patch/cobd-2.6.26.diff
│   ├── ./patch/cobd-2.6.33.diff
│   ├── ./patch/cobd-core.diff
│   ├── ./patch/coconsole-2.6.25.diff
│   ├── ./patch/coconsole-2.6.26.diff
│   ├── ./patch/coconsole-2.6.33.diff
│   ├── ./patch/coconsole-core.diff
│   ├── ./patch/cofs-2.6.25.diff
│   ├── ./patch/cofs-2.6.26.diff
│   ├── ./patch/cofs-2.6.33.diff
│   ├── ./patch/cofs-core.diff
│   ├── ./patch/colinux_2.6.26-ext4-7.diff
│   ├── ./patch/conet-2.6.25.diff
│   ├── ./patch/conet-2.6.26.diff
│   ├── ./patch/conet-2.6.33.diff
│   ├── ./patch/conet-pci-core.diff
│   ├── ./patch/fltk-1.1.10-linux-patch.diff
│   ├── ./patch/fltk-1.1.10-win32.diff
│   ├── ./patch/frame-size-checks.diff
│   ├── ./patch/gcc412fix.diff
│   ├── ./patch/gcc-4.1.2.patch
│   ├── ./patch/kbd-2.6.25.diff
│   ├── ./patch/kbd-2.6.26.diff
│   ├── ./patch/kbd-2.6.33.diff
│   ├── ./patch/kbd-core.diff
│   ├── ./patch/mouse-2.6.25.diff
│   ├── ./patch/mouse-2.6.26.diff
│   ├── ./patch/mouse-2.6.33.diff
│   ├── ./patch/mouse-core.diff
│   ├── ./patch/pci-2.6.25.diff
│   ├── ./patch/pci-2.6.26.diff
│   ├── ./patch/pci-2.6.33.diff
│   ├── ./patch/pci-core.diff
│   ├── ./patch/scsi-2.6.25.diff
│   ├── ./patch/scsi-2.6.26.diff
│   ├── ./patch/scsi-2.6.33.diff
│   ├── ./patch/scsi-core.diff
│   ├── ./patch/serial-2.6.25.diff
│   ├── ./patch/serial-2.6.26.diff
│   ├── ./patch/serial-2.6.33.diff
│   ├── ./patch/serial-core.diff
│   ├── ./patch/series-2.6.25.20
│   ├── ./patch/series-2.6.26.8
│   ├── ./patch/series-2.6.33.7
│   ├── ./patch/squashfs3.3-2.6.25.diff
│   ├── ./patch/squashfs3.3-core.diff
│   ├── ./patch/squashfs3.4-2.6.26.diff
│   ├── ./patch/timer-2.6.25.diff
│   ├── ./patch/timer-2.6.26.diff
│   ├── ./patch/timer-2.6.33.diff
│   ├── ./patch/tty-buffer-alloc-2.6.33.diff
│   ├── ./patch/unionfs-2.5.4_for_2.6.25.20.diff
│   ├── ./patch/unionfs-2.5.4_for_2.6.26.8.diff
│   ├── ./patch/unionfs-2.5.4_for_2.6.33.diff
│   ├── ./patch/unionfs-2.6.25.20.diff
│   ├── ./patch/unionfs-2.6.26.8.diff
│   ├── ./patch/video-2.6.25.diff
│   ├── ./patch/video-2.6.26.diff
│   ├── ./patch/video-2.6.33.diff
│   └── ./patch/video-core.diff
├── ./patched.diff
├── ./patched.diff.out
├── ./README.md
└── ./src
    ├── ./src/build.comake.py
    ├── ./src/colinux
    │   ├── ./src/colinux/arch
    │   │   ├── ./src/colinux/arch/build.comake.py
    │   │   ├── ./src/colinux/arch/current -> i386
    │   │   ├── ./src/colinux/arch/i386
    │   │   │   ├── ./src/colinux/arch/i386/antinx.c
    │   │   │   ├── ./src/colinux/arch/i386/antinx.h
    │   │   │   ├── ./src/colinux/arch/i386/apic.c
    │   │   │   ├── ./src/colinux/arch/i386/apic.h
    │   │   │   ├── ./src/colinux/arch/i386/build.comake.py
    │   │   │   ├── ./src/colinux/arch/i386/cpuid.c
    │   │   │   ├── ./src/colinux/arch/i386/cpuid.h
    │   │   │   ├── ./src/colinux/arch/i386/defs.h
    │   │   │   ├── ./src/colinux/arch/i386/interrupt.c
    │   │   │   ├── ./src/colinux/arch/i386/Makefile
    │   │   │   ├── ./src/colinux/arch/i386/manager.c
    │   │   │   ├── ./src/colinux/arch/i386/manager.h
    │   │   │   ├── ./src/colinux/arch/i386/mmu.h
    │   │   │   ├── ./src/colinux/arch/i386/passage.c
    │   │   │   ├── ./src/colinux/arch/i386/utils.c
    │   │   │   └── ./src/colinux/arch/i386/utils.h
    │   │   ├── ./src/colinux/arch/interrupt.h
    │   │   ├── ./src/colinux/arch/Makefile
    │   │   ├── ./src/colinux/arch/manager.h
    │   │   ├── ./src/colinux/arch/mmu.h
    │   │   └── ./src/colinux/arch/passage.h
    │   ├── ./src/colinux/build.comake.py
    │   ├── ./src/colinux/common
    │   │   ├── ./src/colinux/common/build.comake.py
    │   │   ├── ./src/colinux/common/common_base.h
    │   │   ├── ./src/colinux/common/common.h
    │   │   ├── ./src/colinux/common/config.h
    │   │   ├── ./src/colinux/common/console.c
    │   │   ├── ./src/colinux/common/console.h
    │   │   ├── ./src/colinux/common/debug.c
    │   │   ├── ./src/colinux/common/debug.h
    │   │   ├── ./src/colinux/common/errors.c
    │   │   ├── ./src/colinux/common/import.h
    │   │   ├── ./src/colinux/common/ioctl.h
    │   │   ├── ./src/colinux/common/libc.c
    │   │   ├── ./src/colinux/common/libc.h
    │   │   ├── ./src/colinux/common/list.h
    │   │   ├── ./src/colinux/common/Makefile
    │   │   ├── ./src/colinux/common/messages.c
    │   │   ├── ./src/colinux/common/messages.h
    │   │   ├── ./src/colinux/common/module_repr.c
    │   │   ├── ./src/colinux/common/queue.c
    │   │   ├── ./src/colinux/common/queue.h
    │   │   ├── ./src/colinux/common/snprintf.c
    │   │   ├── ./src/colinux/common/unicode.c
    │   │   ├── ./src/colinux/common/unicode.h
    │   │   └── ./src/colinux/common/version.h
    │   ├── ./src/colinux/kernel
    │   │   ├── ./src/colinux/kernel/block.c
    │   │   ├── ./src/colinux/kernel/block.h
    │   │   ├── ./src/colinux/kernel/build.comake.py
    │   │   ├── ./src/colinux/kernel/debug.c
    │   │   ├── ./src/colinux/kernel/debug.h
    │   │   ├── ./src/colinux/kernel/fileblock.c
    │   │   ├── ./src/colinux/kernel/fileblock.h
    │   │   ├── ./src/colinux/kernel/filesystem.c
    │   │   ├── ./src/colinux/kernel/filesystem.h
    │   │   ├── ./src/colinux/kernel/Makefile
    │   │   ├── ./src/colinux/kernel/manager.c
    │   │   ├── ./src/colinux/kernel/manager.h
    │   │   ├── ./src/colinux/kernel/monitor.c
    │   │   ├── ./src/colinux/kernel/monitor.h
    │   │   ├── ./src/colinux/kernel/pages.c
    │   │   ├── ./src/colinux/kernel/pages.h
    │   │   ├── ./src/colinux/kernel/pci.c
    │   │   ├── ./src/colinux/kernel/pci.h
    │   │   ├── ./src/colinux/kernel/README
    │   │   ├── ./src/colinux/kernel/reversedpfns.c
    │   │   ├── ./src/colinux/kernel/reversedpfns.h
    │   │   ├── ./src/colinux/kernel/scsi.c
    │   │   ├── ./src/colinux/kernel/scsi.h
    │   │   ├── ./src/colinux/kernel/transfer.c
    │   │   ├── ./src/colinux/kernel/transfer.h
    │   │   ├── ./src/colinux/kernel/video.c
    │   │   └── ./src/colinux/kernel/video.h
    │   ├── ./src/colinux/Makefile
    │   ├── ./src/colinux/os
    │   │   ├── ./src/colinux/os/alloc.h
    │   │   ├── ./src/colinux/os/current -> winnt
    │   │   ├── ./src/colinux/os/kernel
    │   │   │   ├── ./src/colinux/os/kernel/alloc.h
    │   │   │   ├── ./src/colinux/os/kernel/filesystem.h
    │   │   │   ├── ./src/colinux/os/kernel/manager.h
    │   │   │   ├── ./src/colinux/os/kernel/misc.h
    │   │   │   ├── ./src/colinux/os/kernel/monitor.h
    │   │   │   ├── ./src/colinux/os/kernel/mutex.h
    │   │   │   ├── ./src/colinux/os/kernel/time.h
    │   │   │   ├── ./src/colinux/os/kernel/user.h
    │   │   │   └── ./src/colinux/os/kernel/wait.h
    │   │   ├── ./src/colinux/os/README
    │   │   ├── ./src/colinux/os/timer.h
    │   │   ├── ./src/colinux/os/user
    │   │   │   ├── ./src/colinux/os/user/cobdpath.h
    │   │   │   ├── ./src/colinux/os/user/config.h
    │   │   │   ├── ./src/colinux/os/user/daemon.h
    │   │   │   ├── ./src/colinux/os/user/exec.h
    │   │   │   ├── ./src/colinux/os/user/file.h
    │   │   │   ├── ./src/colinux/os/user/manager.h
    │   │   │   ├── ./src/colinux/os/user/misc.h
    │   │   │   └── ./src/colinux/os/user/reactor.h
    │   │   └── ./src/colinux/os/winnt
    │   │       ├── ./src/colinux/os/winnt/ioctl.h
    │   │       ├── ./src/colinux/os/winnt/kernel
    │   │       │   ├── ./src/colinux/os/winnt/kernel/alloc.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/block.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/build.comake.py
    │   │       │   ├── ./src/colinux/os/winnt/kernel/conet.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/conet.h
    │   │       │   ├── ./src/colinux/os/winnt/kernel/core.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/ddk.h
    │   │       │   ├── ./src/colinux/os/winnt/kernel/driver.h
    │   │       │   ├── ./src/colinux/os/winnt/kernel/exceptions.h
    │   │       │   ├── ./src/colinux/os/winnt/kernel/fileio.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/fileio.h
    │   │       │   ├── ./src/colinux/os/winnt/kernel/filesystem.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/interface.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/lowlevel
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/lowlevel/alloc.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/lowlevel/build.comake.py
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/lowlevel/debug.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/lowlevel/Makefile
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/lowlevel/misc.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/lowlevel/mutex.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/lowlevel/time.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/lowlevel/timer.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/lowlevel/user.c
    │   │       │   │   └── ./src/colinux/os/winnt/kernel/lowlevel/wait.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/Makefile
    │   │       │   ├── ./src/colinux/os/winnt/kernel/manager.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/manager.h
    │   │       │   ├── ./src/colinux/os/winnt/kernel/scsi.c
    │   │       │   ├── ./src/colinux/os/winnt/kernel/tap-win32
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/common.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/config-win32.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/constants.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/dhcp.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/dhcp.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/endian.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/error.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/error.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/hexdump.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/hexdump.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/i386
    │   │       │   │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/i386/OemWin2k.inf
    │   │       │   │   │   └── ./src/colinux/os/winnt/kernel/tap-win32/i386/tap.cat
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/instance.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/lock.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/macinfo.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/macinfo.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/MAKEFILE
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/mem.c
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/proto.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/prototypes.h
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/resource.rc
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/SOURCES
    │   │       │   │   ├── ./src/colinux/os/winnt/kernel/tap-win32/tapdrvr.c
    │   │       │   │   └── ./src/colinux/os/winnt/kernel/tap-win32/types.h
    │   │       │   ├── ./src/colinux/os/winnt/kernel/time.c
    │   │       │   └── ./src/colinux/os/winnt/kernel/time.h
    │   │       ├── ./src/colinux/os/winnt/memory.h
    │   │       ├── ./src/colinux/os/winnt/monitor.h
    │   │       ├── ./src/colinux/os/winnt/os.h
    │   │       └── ./src/colinux/os/winnt/user
    │   │           ├── ./src/colinux/os/winnt/user/alloc.c
    │   │           ├── ./src/colinux/os/winnt/user/build.comake.py
    │   │           ├── ./src/colinux/os/winnt/user/cobdpath.c
    │   │           ├── ./src/colinux/os/winnt/user/conet-bridged-daemon
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-bridged-daemon/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-bridged-daemon/main.c
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-bridged-daemon/Makefile
    │   │           │   └── ./src/colinux/os/winnt/user/conet-bridged-daemon/pcap-registry.h
    │   │           ├── ./src/colinux/os/winnt/user/conet-daemon
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-daemon/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-daemon/daemon.cpp
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-daemon/daemon.h
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-daemon/Makefile
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-daemon/tap-win32.c
    │   │           │   └── ./src/colinux/os/winnt/user/conet-daemon/tap-win32.h
    │   │           ├── ./src/colinux/os/winnt/user/conet-ndis-daemon
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-ndis-daemon/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-ndis-daemon/main.c
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-ndis-daemon/Makefile
    │   │           │   └── ./src/colinux/os/winnt/user/conet-ndis-daemon/pcap-registry.h
    │   │           ├── ./src/colinux/os/winnt/user/conet-slirp-daemon
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-slirp-daemon/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/conet-slirp-daemon/main.c
    │   │           │   └── ./src/colinux/os/winnt/user/conet-slirp-daemon/Makefile
    │   │           ├── ./src/colinux/os/winnt/user/console-fltk
    │   │           │   ├── ./src/colinux/os/winnt/user/console-fltk/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/console-fltk/head.cpp
    │   │           │   └── ./src/colinux/os/winnt/user/console-fltk/Makefile
    │   │           ├── ./src/colinux/os/winnt/user/console-nt
    │   │           │   ├── ./src/colinux/os/winnt/user/console-nt/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/console-nt/console.cpp
    │   │           │   ├── ./src/colinux/os/winnt/user/console-nt/console.h
    │   │           │   ├── ./src/colinux/os/winnt/user/console-nt/head.cpp
    │   │           │   ├── ./src/colinux/os/winnt/user/console-nt/Makefile
    │   │           │   ├── ./src/colinux/os/winnt/user/console-nt/os_console.c
    │   │           │   ├── ./src/colinux/os/winnt/user/console-nt/os_console.h
    │   │           │   ├── ./src/colinux/os/winnt/user/console-nt/widget.cpp
    │   │           │   └── ./src/colinux/os/winnt/user/console-nt/widget.h
    │   │           ├── ./src/colinux/os/winnt/user/coserial-daemon
    │   │           │   ├── ./src/colinux/os/winnt/user/coserial-daemon/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/coserial-daemon/main.c
    │   │           │   └── ./src/colinux/os/winnt/user/coserial-daemon/Makefile
    │   │           ├── ./src/colinux/os/winnt/user/daemon
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/cmdline.c
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/cmdline.h
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/debug.c
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/debug.h
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/driver.c
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/driver.h
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/main.c
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/main.h
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/Makefile
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/misc.c
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/misc.h
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/registry.c
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/res
    │   │           │   │   ├── ./src/colinux/os/winnt/user/daemon/res/build.comake.py
    │   │           │   │   ├── ./src/colinux/os/winnt/user/daemon/res/colinux.ico
    │   │           │   │   ├── ./src/colinux/os/winnt/user/daemon/res/colinux.rc
    │   │           │   │   ├── ./src/colinux/os/winnt/user/daemon/res/daemon.rc
    │   │           │   │   ├── ./src/colinux/os/winnt/user/daemon/res/Makefile
    │   │           │   │   ├── ./src/colinux/os/winnt/user/daemon/res/resources_def.sh
    │   │           │   │   ├── ./src/colinux/os/winnt/user/daemon/res/service-message.bin
    │   │           │   │   ├── ./src/colinux/os/winnt/user/daemon/res/service-message.h
    │   │           │   │   └── ./src/colinux/os/winnt/user/daemon/res/service-message.mc
    │   │           │   ├── ./src/colinux/os/winnt/user/daemon/service.c
    │   │           │   └── ./src/colinux/os/winnt/user/daemon/service.h
    │   │           ├── ./src/colinux/os/winnt/user/daemon.h
    │   │           ├── ./src/colinux/os/winnt/user/debug
    │   │           │   ├── ./src/colinux/os/winnt/user/debug/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/debug/main.c
    │   │           │   └── ./src/colinux/os/winnt/user/debug/Makefile
    │   │           ├── ./src/colinux/os/winnt/user/exec.c
    │   │           ├── ./src/colinux/os/winnt/user/file.c
    │   │           ├── ./src/colinux/os/winnt/user/file-unlink.c
    │   │           ├── ./src/colinux/os/winnt/user/file-write.c
    │   │           ├── ./src/colinux/os/winnt/user/install
    │   │           │   ├── ./src/colinux/os/winnt/user/install/build.comake.py
    │   │           │   ├── ./src/colinux/os/winnt/user/install/colinux_def.sh
    │   │           │   ├── ./src/colinux/os/winnt/user/install/colinux.nsi
    │   │           │   ├── ./src/colinux/os/winnt/user/install/header.bmp
    │   │           │   ├── ./src/colinux/os/winnt/user/install/iDl.ini
    │   │           │   ├── ./src/colinux/os/winnt/user/install/Makefile
    │   │           │   ├── ./src/colinux/os/winnt/user/install/startlogo.bmp
    │   │           │   ├── ./src/colinux/os/winnt/user/install/WinpcapRedir.ini
    │   │           │   └── ./src/colinux/os/winnt/user/install/WriteEnvStr.nsh
    │   │           ├── ./src/colinux/os/winnt/user/Makefile
    │   │           ├── ./src/colinux/os/winnt/user/manager.c
    │   │           ├── ./src/colinux/os/winnt/user/manager.h
    │   │           ├── ./src/colinux/os/winnt/user/misc.c
    │   │           ├── ./src/colinux/os/winnt/user/misc.h
    │   │           ├── ./src/colinux/os/winnt/user/process.c
    │   │           ├── ./src/colinux/os/winnt/user/reactor.c
    │   │           ├── ./src/colinux/os/winnt/user/reactor.h
    │   │           ├── ./src/colinux/os/winnt/user/timer.c
    │   │           └── ./src/colinux/os/winnt/user/udp.c
    │   ├── ./src/colinux/user
    │   │   ├── ./src/colinux/user/build.comake.py
    │   │   ├── ./src/colinux/user/cmdline.c
    │   │   ├── ./src/colinux/user/cmdline.h
    │   │   ├── ./src/colinux/user/config.c
    │   │   ├── ./src/colinux/user/config.h
    │   │   ├── ./src/colinux/user/configurator
    │   │   │   ├── ./src/colinux/user/configurator/blockdevice.py
    │   │   │   ├── ./src/colinux/user/configurator/common.py
    │   │   │   ├── ./src/colinux/user/configurator/configurator.py
    │   │   │   ├── ./src/colinux/user/configurator/networkdevice.py
    │   │   │   ├── ./src/colinux/user/configurator/README
    │   │   │   └── ./src/colinux/user/configurator/xmlwrapper.py
    │   │   ├── ./src/colinux/user/console-fltk
    │   │   │   ├── ./src/colinux/user/console-fltk/about.cpp
    │   │   │   ├── ./src/colinux/user/console-fltk/about.h
    │   │   │   ├── ./src/colinux/user/console-fltk/build.comake.py
    │   │   │   ├── ./src/colinux/user/console-fltk/console.cpp
    │   │   │   ├── ./src/colinux/user/console-fltk/console.h
    │   │   │   ├── ./src/colinux/user/console-fltk/fontselect.cpp
    │   │   │   ├── ./src/colinux/user/console-fltk/fontselect.h
    │   │   │   ├── ./src/colinux/user/console-fltk/img
    │   │   │   │   ├── ./src/colinux/user/console-fltk/img/colinux_logo_3d.xpm
    │   │   │   │   └── ./src/colinux/user/console-fltk/img/colinux_logo.xpm
    │   │   │   ├── ./src/colinux/user/console-fltk/input.cpp
    │   │   │   ├── ./src/colinux/user/console-fltk/input.h
    │   │   │   ├── ./src/colinux/user/console-fltk/log_window.cpp
    │   │   │   ├── ./src/colinux/user/console-fltk/log_window.h
    │   │   │   ├── ./src/colinux/user/console-fltk/main.cpp
    │   │   │   ├── ./src/colinux/user/console-fltk/main.h
    │   │   │   ├── ./src/colinux/user/console-fltk/Makefile
    │   │   │   ├── ./src/colinux/user/console-fltk/select_monitor.cpp
    │   │   │   ├── ./src/colinux/user/console-fltk/select_monitor.h
    │   │   │   ├── ./src/colinux/user/console-fltk/widget.cpp
    │   │   │   └── ./src/colinux/user/console-fltk/widget.h
    │   │   ├── ./src/colinux/user/console-nt
    │   │   │   ├── ./src/colinux/user/console-nt/build.comake.py
    │   │   │   ├── ./src/colinux/user/console-nt/console.cpp
    │   │   │   ├── ./src/colinux/user/console-nt/console.h
    │   │   │   ├── ./src/colinux/user/console-nt/main.cpp
    │   │   │   ├── ./src/colinux/user/console-nt/main.h
    │   │   │   ├── ./src/colinux/user/console-nt/Makefile
    │   │   │   ├── ./src/colinux/user/console-nt/widget.cpp
    │   │   │   └── ./src/colinux/user/console-nt/widget.h
    │   │   ├── ./src/colinux/user/context.h
    │   │   ├── ./src/colinux/user/control
    │   │   │   └── ./src/colinux/user/control/main.c
    │   │   ├── ./src/colinux/user/daemon-base
    │   │   │   ├── ./src/colinux/user/daemon-base/build.comake.py
    │   │   │   ├── ./src/colinux/user/daemon-base/main.c
    │   │   │   ├── ./src/colinux/user/daemon-base/main.cpp
    │   │   │   ├── ./src/colinux/user/daemon-base/main.h
    │   │   │   └── ./src/colinux/user/daemon-base/Makefile
    │   │   ├── ./src/colinux/user/daemon.c
    │   │   ├── ./src/colinux/user/daemon.h
    │   │   ├── ./src/colinux/user/debug
    │   │   │   ├── ./src/colinux/user/debug/build.comake.py
    │   │   │   ├── ./src/colinux/user/debug/dump.py
    │   │   │   ├── ./src/colinux/user/debug/main.c
    │   │   │   ├── ./src/colinux/user/debug/main.h
    │   │   │   ├── ./src/colinux/user/debug/Makefile
    │   │   │   └── ./src/colinux/user/debug/server.py
    │   │   ├── ./src/colinux/user/debug.c
    │   │   ├── ./src/colinux/user/debug.h
    │   │   ├── ./src/colinux/user/elf_load.c
    │   │   ├── ./src/colinux/user/elf_load.h
    │   │   ├── ./src/colinux/user/macaddress.c
    │   │   ├── ./src/colinux/user/macaddress.h
    │   │   ├── ./src/colinux/user/Makefile
    │   │   ├── ./src/colinux/user/manager.c
    │   │   ├── ./src/colinux/user/manager.h
    │   │   ├── ./src/colinux/user/monitor.c
    │   │   ├── ./src/colinux/user/monitor-find.c
    │   │   ├── ./src/colinux/user/monitor.h
    │   │   ├── ./src/colinux/user/pidfile.c
    │   │   ├── ./src/colinux/user/reactor.c
    │   │   ├── ./src/colinux/user/reactor.h
    │   │   ├── ./src/colinux/user/README
    │   │   └── ./src/colinux/user/slirp
    │   │       ├── ./src/colinux/user/slirp/bootp.c
    │   │       ├── ./src/colinux/user/slirp/bootp.h
    │   │       ├── ./src/colinux/user/slirp/build.comake.py
    │   │       ├── ./src/colinux/user/slirp/cksum.c
    │   │       ├── ./src/colinux/user/slirp/co_main.c
    │   │       ├── ./src/colinux/user/slirp/co_main.h
    │   │       ├── ./src/colinux/user/slirp/config.h
    │   │       ├── ./src/colinux/user/slirp/COPYRIGHT
    │   │       ├── ./src/colinux/user/slirp/ctl.h
    │   │       ├── ./src/colinux/user/slirp/debug.c
    │   │       ├── ./src/colinux/user/slirp/debug.h
    │   │       ├── ./src/colinux/user/slirp/icmp_var.h
    │   │       ├── ./src/colinux/user/slirp/if.c
    │   │       ├── ./src/colinux/user/slirp/if.h
    │   │       ├── ./src/colinux/user/slirp/ip.h
    │   │       ├── ./src/colinux/user/slirp/ip_icmp.c
    │   │       ├── ./src/colinux/user/slirp/ip_icmp.h
    │   │       ├── ./src/colinux/user/slirp/ip_input.c
    │   │       ├── ./src/colinux/user/slirp/ip_output.c
    │   │       ├── ./src/colinux/user/slirp/libslirp.h
    │   │       ├── ./src/colinux/user/slirp/main.h
    │   │       ├── ./src/colinux/user/slirp/Makefile
    │   │       ├── ./src/colinux/user/slirp/mbuf.c
    │   │       ├── ./src/colinux/user/slirp/mbuf.h
    │   │       ├── ./src/colinux/user/slirp/misc.c
    │   │       ├── ./src/colinux/user/slirp/misc.h
    │   │       ├── ./src/colinux/user/slirp/sbuf.c
    │   │       ├── ./src/colinux/user/slirp/sbuf.h
    │   │       ├── ./src/colinux/user/slirp/slirp.c
    │   │       ├── ./src/colinux/user/slirp/slirp_config.h
    │   │       ├── ./src/colinux/user/slirp/slirp.h
    │   │       ├── ./src/colinux/user/slirp/socket.c
    │   │       ├── ./src/colinux/user/slirp/socket.h
    │   │       ├── ./src/colinux/user/slirp/tcp.h
    │   │       ├── ./src/colinux/user/slirp/tcp_input.c
    │   │       ├── ./src/colinux/user/slirp/tcpip.h
    │   │       ├── ./src/colinux/user/slirp/tcp_output.c
    │   │       ├── ./src/colinux/user/slirp/tcp_subr.c
    │   │       ├── ./src/colinux/user/slirp/tcp_timer.c
    │   │       ├── ./src/colinux/user/slirp/tcp_timer.h
    │   │       ├── ./src/colinux/user/slirp/tcp_var.h
    │   │       ├── ./src/colinux/user/slirp/tftp.c
    │   │       ├── ./src/colinux/user/slirp/tftp.h
    │   │       ├── ./src/colinux/user/slirp/udp.c
    │   │       └── ./src/colinux/user/slirp/udp.h
    │   └── ./src/colinux/VERSION
    ├── ./src/Makefile
    └── ./src/Makefile.old

44 directories, 470 files
