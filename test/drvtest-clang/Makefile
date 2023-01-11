#
# by Frank Rysanek <rysanek@fccps.cz>
#
# Based on the source code of ioperm.c by Marcel Telka <marcel@telka.sk> 
#   - thanks a million :-)
#

# See also my_names.h
# Note that after changing the driver name, you also have
# to rename $(DRVNAME).c manually in this directory :-)
DRVNAME = drvtest

PREFIX = ${HOME}/llvm-mingw

#INCLUDES = -I/usr/include/w32api/ddk
INCLUDES = -I$(PREFIX)/i686-w64-mingw32/include/ddk

# We could in fact just add -DMY_DRIVER_NAME=\"$(DRVNAME)\" to CFLAGS,
# but we'd have to be careful to "make clean" after changing 
# the driver name here in the makefile...
#CFLAGS = -Wall $(INCLUDES) -DMY_DRIVER_NAME=\"$(DRVNAME)\"
CFLAGS = -Wall $(INCLUDES) -g -gcodeview

# Kernel-mode libs: 
#   libntoskrnl = basic kernel-mode environment
#   libhal = WRITE_PORT_UCHAR et al.
#KRNLIBS = -lntoskrnl -lhal
KRNLIBS = -lntoskrnl

Q = $(PREFIX)/bin/i686-w64-mingw32-
CC = $(Q)gcc
DLLTOOL = $(Q)dlltool
STRIP = strip # Don't use LLVM-MinGW's strip!!!

all: $(DRVNAME).sys loader.exe test_app.exe

# Dependencies on header files:

err_helpers.o: err_helpers.c
	$(CC) $(CFLAGS) -c -o $@ $<

loader.o: loader.c
	$(CC) $(CFLAGS) -c -o $@ $<

loader.exe: loader.o err_helpers.o
	$(CC) $(CFLAGS) -o $@ $^

test_app.exe: test_app.o err_helpers.o
	$(CC) $(CFLAGS) -o $@ $^

$(DRVNAME).o: $(DRVNAME).c
	$(CC) $(CFLAGS) -c -o $@ $<

# This inference rule allows us to turn an .o into a .sys without
# much hassle, implicitly.
# The downside is, that you cannot potentially add further object files
# to get linked into the .sys driver (such as, some custom ASM routines).
# Oh wait, maybe you can... try adding your .o after the last $(CC) in the rule..
$(DRVNAME).sys: $(DRVNAME).o
	$(CC) \
		-shared \
		-Wl,/dynamicbase \
		-Wl,--nxcompat \
		-Wl,/DRIVER \
		-WL,/kernel \
		-Wl,--subsystem,native \
		-Wl,--image-base,0x10000000 \
		-Wl,--file-alignment,0x1000 \
		-Wl,--section-alignment,0x1000 \
		-Wl,--entry,_DriverEntry@8 \
		-Wl,--pdb=$(DRVNAME).pdb \
		-nostartfiles -nostdlib \
		-o $(DRVNAME).sys \
		$(CFLAGS) \
		$(DRVNAME).o \
		$(KRNLIBS)
#	$(STRIP) --keep-file-symbols $(DRVNAME).sys
	../checksum/checksum $(DRVNAME).sys

JUNK = *.base *.exp *.o *~ junk.tmp

clean:
	rm -f $(JUNK) *.sys *.exe *.pdb

semiclean:
	rm -f $(JUNK) 

