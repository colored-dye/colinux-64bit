## Some ideas

0. *Next step*: Build MinGW with GCC-4.1.2 on Ubuntu 10.04 32-bit.
	Build GCC-4.1.2 with GCC(Ubuntu 4.1.2-27ubuntu1), but use system built-in binutils.
	Can be used to build 2.6.33.7 kernel with i386_defconfig.
	OR: review newer cooperative 4.9.325 kernel and build 32-bit coLinux running on 64-bit Windows systems like Windows 10.

1. About MinGW: The MinGW originally used (MinGW 3.1.4) can only be integrated with gcc-4.1.2, not gcc-4.4.x and above(not thoroughly tested). Switch to MinGW-w64, which would result in some changes not limited to driver-related structs and APIs.

2. About Linux kernel: Try v4.x kernel, e.g. 4.9.325 LTS. Better compatibility for more up-to-date compilers.

3. Kernel code: Modify kernel 4.9.325. (**Finished 1st pass**)

4. Building: Build coLinux AFTER building kernel.

## Dependencies

1.   GCC: https://ftp.gnu.org/pub/gnu/gcc/
2.   MinGW-w64: https://sourceforge.net/projects/mingw-w64/files/mingw-w64/mingw-w64-release/mingw-w64-v10.0.0.tar.bz2
3. GNU binutils: https://ftp.gnu.org/pub/gnu/binutils
4.   WinPCAP: https://winpcap.org/archive/4.1.1-WpdPack.zip
5.   FLTK: https://www.fltk.org/pub/fltk/1.3.8/fltk-1.3.8-source.tar.bz2
