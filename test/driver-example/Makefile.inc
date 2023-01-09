PREFIX = $(abspath ../../mingw/i686)

Q = $(PREFIX)/bin/i686-w64-mingw32-
CC = gcc

CFLAGS := -Wall -Wextra -Wno-sign-compare -Wno-strict-aliasing \
        -fPIC -fvisibility=hidden \
        -ffunction-sections -fdata-sections -fno-builtin -ffreestanding \
        -fno-stack-protector -mno-stack-arg-probe \
        -I$(PREFIX)/i686-w64-mingw32/include/ddk \
	-gstrict-dwarf -O0 -fno-omit-frame-pointer

DRIVER_LDFLAGS := -shared \
        -Wl,--subsystem,native \
        -Wl,--dynamicbase \
	-Wl,--nxcompat \
        -Wl,--image-base,0x10000 \
        -Wl,--file-alignment,0x1000 \
        -Wl,--section-alignment,0x1000 \
        -Wl,--entry,_DriverEntry@8 \
        -nostartfiles -nodefaultlibs -nostdlib

DRIVER_LIBS := -lntoskrnl

define BUILD_C_OBJECT
	$(Q)$(CC) $(CFLAGS) -c $(1) -o $(2)
	@echo 'CC  $(2)'
endef

define LINK_C_KERNEL_TARGET
	$(Q)$(CC) \
		$(CFLAGS) \
		$(DRIVER_LDFLAGS) \
		-Wl,-Map='$(2).map' \
		-o '$(2)' \
		$(1) \
		$(DRIVER_LIBS)
	@echo 'LD $(2)'
endef

