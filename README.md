## Some ideas

1. Project management tool: CMake/Makefile. The Python scripts by Da-x seems somehow overwhelming for me.
2. Compile a 32-bit driver that can actually run on Windows 11.
3. The MinGW originally used (MinGW 3.1.4) can only be integrated with gcc-4.1.2 but not gcc-4.4.x and above(not thoroughly tested). Switch to MinGW-w64, which would result in some changes not limited to driver-related header configuration.
4. About Linux kernel: Try v4.x kernel, e.g. 4.9.325 LTS. Better compatibility for more up-to-date compilers.
5. Next step: Patch kernel 4.9.325. (Up to: arch/x86/mm/init_32.c)
6. Build coLinux AFTER building kernel.

## Dependencies

1.   GCC: https://ftp.gnu.org/pub/gnu/gcc/
2.   MinGW-w64: https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v10.0.0.tar.bz2
3. GNU binutils: https://ftp.gnu.org/pub/gnu/binutils
4.   WinPCAP: https://winpcap.org/archive/4.1.1-WpdPack.zip
5.   FLTK: https://www.fltk.org/pub/fltk/1.3.8/fltk-1.3.8-source.tar.bz2
