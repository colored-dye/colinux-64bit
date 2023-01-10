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

