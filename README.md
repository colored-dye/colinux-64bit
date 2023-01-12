## Some ideas

0. **Next step**: 

Setup development environment on Linux with Windows VM; most importantly -- kernel driver debugging. Remote debug test machine from debugger.

1. About MinGW: The MinGW originally used (MinGW 3.1.4) can only be integrated with gcc-4.1.2, not gcc-4.4.x and above(not thoroughly tested). Switch to MinGW-w64, which would result in some changes not limited to driver-related structs and APIs.

    Now (1/6/2023): No more MinGW. Use MSVC on Visual Studio for better debugging support.

2. About Linux kernel: Try v4/v5 kernel, e.g. 4.9.325 LTS, with better compatibility for newer compilers. (**Finished 1st pass**)

3. About Windows kernel mode driver: 32-bit drivers can only be loaded by 32-bit systems. Test on 32-bit Windows XP first for functionality correctness.

4. Building: Build coLinux AFTER building kernel.

## Dependencies

1.   GCC: https://ftp.gnu.org/pub/gnu/gcc/
2.   MinGW-w64: https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v10.0.0.tar.bz2
3. GNU binutils: https://ftp.gnu.org/pub/gnu/binutils
4.   WinPCAP: https://winpcap.org/archive/4.1.1-WpdPack.zip
5.   FLTK: https://www.fltk.org/pub/fltk/1.3.8/fltk-1.3.8-source.tar.bz2
