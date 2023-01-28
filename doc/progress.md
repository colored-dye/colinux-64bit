[2023/01/19] Build system: Python + Makefile + Bash script does not work.

From 0.7.1 version source on, the developers used Python and Makefile for build system, and bash scripts for environment variables. A problem is that Makefile cannot "source" variables from bash files. The right way is to source `build-common.sh` from shell before calling `make` from the same shell.

====

[2023/01/11] Clang driver "Error 193: Not a valid Win32 application" problem solved.

It is known that Windows checks the checksum of any code that gets loaded into the kernel. So when our clang compiler fails to write the correct checksum into the DLL driver, Windows would report an error and refuse to load the driver.

The simple fix is to use `strip` program. But I want a better fix. So I choose to write a simple program to modify the driver's checksum. After correcting the checksum, the driver could be successfully loaded.

Wrote a simple program to check, calculate and correct checksums for PE32 files. See `test/checksum`, called by `drvtest-clang`.

Next: Finish and test build system.

====

[2023/01/10] Use cv2pdb to create PDB debug symbols for Windows kernel driver. A simple "-g" is enough.

Next: Automate cv2pdb. Using Wine?

Wine too heavy, try LLVM-MinGW first. => Succeeded in obtaining PDB symbols for drivers using LLVM-MinGW. (Shown in the `test/drvtest-clang' example)

Problem: LLVM-MinGW-generated driver has results in blue screen. However, after stripping the driver with MinGW/Linux `strip`, driver works!

LLVM-MinGW version: llvm-mingw-20220906-msvcrt-ubuntu-18.04-x86_64.tar.xz

*Maybe* because of `checksum=0', which can be examined with objdump with -x option.

Possible fix: Manually compile LLVM-MinGW. => Doesn't work.

TODO: Try modifying clang source code?

====

[2023/01/09] Use clang to generate PDB debug symbols!

Dependencies: LLVM-MinGW(https://github.com/mstorsjo/llvm-mingw)

Problem: Only works for user programs. Failed to produce correct driver.

Fix: Forget about clang. Use cv2pdb tool (rainers/cv2pdb) to convert DWARF symbols to PDB format, and then strip the DWARF symbols from the executable file.

