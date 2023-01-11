PREFIX = $(abspath ${HOME}/llvm-mingw)

Q = $(PREFIX)/bin/i686-w64-mingw32-
CC = $(Q)gcc

CFLAGS := -Wall -Wextra -Wno-sign-compare -Wno-strict-aliasing \
        -fPIC -fvisibility=hidden \
        -ffunction-sections -fdata-sections -fno-builtin -ffreestanding \
        -fno-stack-protector -mno-stack-arg-probe \
        -I$(PREFIX)/i686-w64-mingw32/include/ddk

# CFLAGS += -g

DRIVER_LDFLAGS := -shared \
	-Wl,/driver \
	-Wl,/kernel \
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
	$(CC) $(CFLAGS) -c $(1) -o $(2)
	@echo 'CC $(2)'
endef

define LINK_C_KERNEL_TARGET
	$(CC) \
		$(CFLAGS) \
		$(DRIVER_LDFLAGS) \
		-Wl,-Map='$(2).map' \
		-o '$(2)' \
		$(1) \
		$(DRIVER_LIBS)
	@echo 'LD $(2)'
	../checksum/checksum $(2)
	@echo 'checksum $(2)'
endef

