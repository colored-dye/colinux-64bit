## mingw32

Linux kernel 2.6.33.7: GCC-4.1.2
coLinux: GCC-4.1.2 with MinGW-3.14 and W32API-3.13

To build gcc-4.1.2 MinGW cross compiler and guest compiler, 
we first need a gcc-4.4.7/4.1.2(Ubuntu 4.1.2-27ubuntu1) compiler.

The gcc-4.4.7/4.1.2 compiler is installed through Ubuntu archives
of trusty:i386.

Before building gcc-4.1.2 with gcc-4.4.7, the source files require patching.

## comake build system

Use Python3 scripts to collect source files, compile, add trace code, etc. Modified from the old coLinux comake build files.
