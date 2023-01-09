PREFIX = ${HOME}/llvm-mingw
CROSS = $(PREFIX)/bin/i686-w64-mingw32-
CC = $(CROSS)gcc
name = test

.PHONY: all clean
all:
	$(CC) $(name).c -o $(name).exe -lntdll -g -gcodeview -fuse-ld=lld -Wl,--pdb=$(name).pdb

clean:
	rm $(name).c $(name).pdb $(name).exe

