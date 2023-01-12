Outline structure (Reference `src/colinux/os/winnt/build/build.comake.py`):

## Essential

1. `colinux-daemon`: Daemon program.

    Depends:

    - `src/colinux/os/winnt/user/daemon/res/daemon.res`
    - `src/colinux/os/winnt/daemon/daemon.o`
    - `src/colinux/os/winnt/user/user-all.a`

2. `linux.sys`: Kernel mode driver.

    Depends:

    - `src/colinux/kernel/build.o`
    - `src/colinux/os/winnt/kernel/build.o`
    - `src/colinux/arch/build.o`
    - `src/colinux/common/common.o`

3. `vmlinux`: Linux kernel image.

    Depends:

    - `$COLINUX_TARGET_KERNEL_SOURCE` / `$COLINUX_TARGET_KERNEL_BUILD`

4. `colinux-console-fltk`

    Depends:

    - `src/colinux/os/winnt/user/daemon/res/colinux-fltk.res`
    - `src/colinux/os/winnt/user/console-fltk/build.a`
    - `src/colinux/os/winnt/user/user-all.a`
    - `src/colinux/user/console-fltk/build.a`
    
