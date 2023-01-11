CROSS := i686-w64-mingw32-
CC := gcc
CXX := g++
AR := ar
LD := ld
TARGET_KERNEL_BUILD := linux-2.5.33.7-build
CFLAGS := -Isrc \
		-I$(TARGET_KERNEL_BUILD)/include \
		-I$(TARGET_KERNEL_BUILD)/arch/x86/include
export CROSS CC CXX AR LD CFLAGS

.PHONY: build
build:
	make -C colinux/os/winnt/build executables
