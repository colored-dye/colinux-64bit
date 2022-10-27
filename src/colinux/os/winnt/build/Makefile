executables: colinux-daemon.exe \
			colinux-debug-daemon.exe \
			linux.sys

colinux-daemon.exe: ../user/daemon/daemon.o ../user/daemon/res/daemon.res
	$(CROSS)$(CC) -o $@ $^ $(CFLAGS)

colinux-debug-daemon.exe: ../user/debug/build.o ../../../user/debug/build.o ../user/daemon/res/colinux-debug.res
	$(CROSS)$(CC) -o $@ $^ $(CFLAGS)

linux.sys: driver.o driver.base.exp
	$(CROSS)$(CC) \
		-Wl,--strip-debug \
		-Wl,--subsystem,native \
		-Wl,--image-base,0x10000 \
		-Wl,--file-alignment,0x1000 \
		-Wl,--section-alignment,0x1000 \
		-Wl,--entry,_DriverEntry@8 \
		-Wl,driver.base.exp \
		-mdll -nostartfiles -nostdlib \
		-o $@ driver.o -lndis -lntoskrnl -lhal -lgcc

driver.base.exp: driver.base.tmp
	$(CROSS)dlltool --dllname linux.sys \
		--base-file $^ \
		--output-exp $@

driver.base.tmp: driver.o
	$(CROSS)$(CC) \
		-Wl,--base-file,$@ \
		-Wl,--entry,_DriverEntry@8 \
		-nostartfiles -nostdlib \
		-o junk.tmp $^ -lndis -lntoskrnl -lhal -lgcc ; \
		rm -f junk.tmp

driver.o: ../../../kernel/build.o ../kernel/build.o ../../../arch/build.o ../../../common/common.a
	$(CROSS)$(LD) -r -o $@ $^
