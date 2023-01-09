2023/01/09: Use clang to generate PDB debug symbols!

Dependencies: LLVM-MinGW(https://github.com/mstorsjo/llvm-mingw)

Problem: Only works for user programs. Failed to produce correct driver.

Fix: Use cv2pdb tool (rainers/cv2pdb) to convert DWARF symbols to PDB format, and then strip the DWARF symbols from the executable file.

