--- linux-2.6.33.7-source/arch/x86/boot/header.S	2022-08-08 11:45:13.635120100 +0800
+++ linux-4.9.325-source/arch/x86/boot/header.S	2022-11-01 13:41:06.989120200 +0800
@@ -21,6 +21,7 @@
 #include <asm/e820.h>
 #include <asm/page_types.h>
 #include <asm/setup.h>
+#include <asm/bootparam.h>
 #include "boot.h"
 #include "voffset.h"
 #include "zoffset.h"
@@ -32,10 +33,6 @@
 #define SVGA_MODE ASK_VGA
 #endif
 
-#ifndef RAMDISK
-#define RAMDISK 0
-#endif
-
 #ifndef ROOT_RDONLY
 #define ROOT_RDONLY 1
 #endif
@@ -45,6 +42,11 @@
 
 	.global bootsect_start
 bootsect_start:
+#ifdef CONFIG_EFI_STUB
+	# "MZ", MS-DOS header
+	.byte 0x4d
+	.byte 0x5a
+#endif
 
 	# Normalize the start address
 	ljmp	$BOOTSEG, $start2
@@ -79,19 +81,202 @@
 	# invoke the BIOS reset code...
 	ljmp	$0xf000,$0xfff0
 
+#ifdef CONFIG_EFI_STUB
+	.org	0x3c
+	#
+	# Offset to the PE header.
+	#
+	.long	pe_header
+#endif /* CONFIG_EFI_STUB */
+
 	.section ".bsdata", "a"
 bugger_off_msg:
-	.ascii	"Direct booting from floppy is no longer supported.\r\n"
-	.ascii	"Please use a boot loader program instead.\r\n"
+	.ascii	"Use a boot loader.\r\n"
 	.ascii	"\n"
-	.ascii	"Remove disk and press any key to reboot . . .\r\n"
+	.ascii	"Remove disk and press any key to reboot...\r\n"
+	.byte	0
+
+#ifdef CONFIG_EFI_STUB
+pe_header:
+	.ascii	"PE"
+	.word 	0
+
+coff_header:
+#ifdef CONFIG_X86_32
+	.word	0x14c				# i386
+#else
+	.word	0x8664				# x86-64
+#endif
+	.word	4				# nr_sections
+	.long	0 				# TimeDateStamp
+	.long	0				# PointerToSymbolTable
+	.long	1				# NumberOfSymbols
+	.word	section_table - optional_header	# SizeOfOptionalHeader
+#ifdef CONFIG_X86_32
+	.word	0x306				# Characteristics.
+						# IMAGE_FILE_32BIT_MACHINE |
+						# IMAGE_FILE_DEBUG_STRIPPED |
+						# IMAGE_FILE_EXECUTABLE_IMAGE |
+						# IMAGE_FILE_LINE_NUMS_STRIPPED
+#else
+	.word	0x206				# Characteristics
+						# IMAGE_FILE_DEBUG_STRIPPED |
+						# IMAGE_FILE_EXECUTABLE_IMAGE |
+						# IMAGE_FILE_LINE_NUMS_STRIPPED
+#endif
+
+optional_header:
+#ifdef CONFIG_X86_32
+	.word	0x10b				# PE32 format
+#else
+	.word	0x20b 				# PE32+ format
+#endif
+	.byte	0x02				# MajorLinkerVersion
+	.byte	0x14				# MinorLinkerVersion
+
+	# Filled in by build.c
+	.long	0				# SizeOfCode
+
+	.long	0				# SizeOfInitializedData
+	.long	0				# SizeOfUninitializedData
+
+	# Filled in by build.c
+	.long	0x0000				# AddressOfEntryPoint
+
+	.long	0x0200				# BaseOfCode
+#ifdef CONFIG_X86_32
+	.long	0				# data
+#endif
+
+extra_header_fields:
+#ifdef CONFIG_X86_32
+	.long	0				# ImageBase
+#else
+	.quad	0				# ImageBase
+#endif
+	.long	0x20				# SectionAlignment
+	.long	0x20				# FileAlignment
+	.word	0				# MajorOperatingSystemVersion
+	.word	0				# MinorOperatingSystemVersion
+	.word	0				# MajorImageVersion
+	.word	0				# MinorImageVersion
+	.word	0				# MajorSubsystemVersion
+	.word	0				# MinorSubsystemVersion
+	.long	0				# Win32VersionValue
+
+	#
+	# The size of the bzImage is written in tools/build.c
+	#
+	.long	0				# SizeOfImage
+
+	.long	0x200				# SizeOfHeaders
+	.long	0				# CheckSum
+	.word	0xa				# Subsystem (EFI application)
+	.word	0				# DllCharacteristics
+#ifdef CONFIG_X86_32
+	.long	0				# SizeOfStackReserve
+	.long	0				# SizeOfStackCommit
+	.long	0				# SizeOfHeapReserve
+	.long	0				# SizeOfHeapCommit
+#else
+	.quad	0				# SizeOfStackReserve
+	.quad	0				# SizeOfStackCommit
+	.quad	0				# SizeOfHeapReserve
+	.quad	0				# SizeOfHeapCommit
+#endif
+	.long	0				# LoaderFlags
+	.long	0x6				# NumberOfRvaAndSizes
+
+	.quad	0				# ExportTable
+	.quad	0				# ImportTable
+	.quad	0				# ResourceTable
+	.quad	0				# ExceptionTable
+	.quad	0				# CertificationTable
+	.quad	0				# BaseRelocationTable
+
+	# Section table
+section_table:
+	#
+	# The offset & size fields are filled in by build.c.
+	#
+	.ascii	".setup"
+	.byte	0
+	.byte	0
+	.long	0
+	.long	0x0				# startup_{32,64}
+	.long	0				# Size of initialized data
+						# on disk
+	.long	0x0				# startup_{32,64}
+	.long	0				# PointerToRelocations
+	.long	0				# PointerToLineNumbers
+	.word	0				# NumberOfRelocations
+	.word	0				# NumberOfLineNumbers
+	.long	0x60500020			# Characteristics (section flags)
+
+	#
+	# The EFI application loader requires a relocation section
+	# because EFI applications must be relocatable. The .reloc
+	# offset & size fields are filled in by build.c.
+	#
+	.ascii	".reloc"
+	.byte	0
+	.byte	0
+	.long	0
+	.long	0
+	.long	0				# SizeOfRawData
+	.long	0				# PointerToRawData
+	.long	0				# PointerToRelocations
+	.long	0				# PointerToLineNumbers
+	.word	0				# NumberOfRelocations
+	.word	0				# NumberOfLineNumbers
+	.long	0x42100040			# Characteristics (section flags)
+
+	#
+	# The offset & size fields are filled in by build.c.
+	#
+	.ascii	".text"
+	.byte	0
 	.byte	0
+	.byte	0
+	.long	0
+	.long	0x0				# startup_{32,64}
+	.long	0				# Size of initialized data
+						# on disk
+	.long	0x0				# startup_{32,64}
+	.long	0				# PointerToRelocations
+	.long	0				# PointerToLineNumbers
+	.word	0				# NumberOfRelocations
+	.word	0				# NumberOfLineNumbers
+	.long	0x60500020			# Characteristics (section flags)
+
+	#
+	# The offset & size fields are filled in by build.c.
+	#
+	.ascii	".bss"
+	.byte	0
+	.byte	0
+	.byte	0
+	.byte	0
+	.long	0
+	.long	0x0
+	.long	0				# Size of initialized data
+						# on disk
+	.long	0x0
+	.long	0				# PointerToRelocations
+	.long	0				# PointerToLineNumbers
+	.word	0				# NumberOfRelocations
+	.word	0				# NumberOfLineNumbers
+	.long	0xc8000080			# Characteristics (section flags)
 
+#endif /* CONFIG_EFI_STUB */
 
 	# Kernel attributes; used by setup.  This is part 1 of the
 	# header, from the old boot sector.
 
 	.section ".header", "a"
+	.globl	sentinel
+sentinel:	.byte 0xff, 0xff        /* Used to detect broken loaders */
+
 	.globl	hdr
 hdr:
 setup_sects:	.byte 0			/* Filled in by build.c */
@@ -116,7 +301,7 @@
 	# Part 2 of the header, from the old setup.S
 
 		.ascii	"HdrS"		# header signature
-		.word	0x020a		# header version number (>= 0x0105)
+		.word	0x020d		# header version number (>= 0x0105)
 					# or else old loadlin-1.5 will fail)
 		.globl realmode_swtch
 realmode_swtch:	.word	0, 0		# default_switch, SETUPSEG
@@ -129,18 +314,12 @@
 
 type_of_loader:	.byte	0		# 0 means ancient bootloader, newer
 					# bootloaders know to change this.
-					# See Documentation/i386/boot.txt for
+					# See Documentation/x86/boot.txt for
 					# assigned ids
 
 # flags, unused bits must be zero (RFU) bit within loadflags
 loadflags:
-LOADED_HIGH	= 1			# If set, the kernel is loaded high
-CAN_USE_HEAP	= 0x80			# If set, the loader also has set
-					# heap_end_ptr to tell how much
-					# space behind setup.S can be used for
-					# heap purposes.
-					# Only the loader knows what is free
-		.byte	LOADED_HIGH
+		.byte	LOADED_HIGH	# The kernel is to be loaded high
 
 setup_move_size: .word  0x8000		# size to move, when setup is not
 					# loaded at 0x90000. We will move setup
@@ -189,7 +368,7 @@
 					# can be located anywhere in
 					# low memory 0x10000 or higher.
 
-ramdisk_max:	.long 0x7fffffff
+initrd_addr_max: .long 0x7fffffff
 					# (Header version 0x0203 or later)
 					# The highest safe address for
 					# the contents of an initrd
@@ -206,7 +385,42 @@
 relocatable_kernel:    .byte 0
 #endif
 min_alignment:		.byte MIN_KERNEL_ALIGN_LG2	# minimum alignment
-pad3:			.word 0
+
+xloadflags:
+#ifdef CONFIG_X86_64
+# define XLF0 XLF_KERNEL_64			/* 64-bit kernel */
+#else
+# define XLF0 0
+#endif
+
+#if defined(CONFIG_RELOCATABLE) && defined(CONFIG_X86_64)
+   /* kernel/boot_param/ramdisk could be loaded above 4g */
+# define XLF1 XLF_CAN_BE_LOADED_ABOVE_4G
+#else
+# define XLF1 0
+#endif
+
+#ifdef CONFIG_EFI_STUB
+# ifdef CONFIG_EFI_MIXED
+#  define XLF23 (XLF_EFI_HANDOVER_32|XLF_EFI_HANDOVER_64)
+# else
+#  ifdef CONFIG_X86_64
+#   define XLF23 XLF_EFI_HANDOVER_64		/* 64-bit EFI handover ok */
+#  else
+#   define XLF23 XLF_EFI_HANDOVER_32		/* 32-bit EFI handover ok */
+#  endif
+# endif
+#else
+# define XLF23 0
+#endif
+
+#if defined(CONFIG_X86_64) && defined(CONFIG_EFI) && defined(CONFIG_KEXEC_CORE)
+# define XLF4 XLF_EFI_KEXEC
+#else
+# define XLF4 0
+#endif
+
+			.word XLF0 | XLF1 | XLF23 | XLF4
 
 cmdline_size:   .long   COMMAND_LINE_SIZE-1     #length of the command line,
                                                 #added with boot protocol
@@ -226,26 +440,123 @@
 
 pref_address:		.quad LOAD_PHYSICAL_ADDR	# preferred load addr
 
-#define ZO_INIT_SIZE	(ZO__end - ZO_startup_32 + ZO_z_extract_offset)
+#
+# Getting to provably safe in-place decompression is hard. Worst case
+# behaviours need to be analyzed. Here let's take the decompression of
+# a gzip-compressed kernel as example, to illustrate it:
+#
+# The file layout of gzip compressed kernel is:
+#
+#    magic[2]
+#    method[1]
+#    flags[1]
+#    timestamp[4]
+#    extraflags[1]
+#    os[1]
+#    compressed data blocks[N]
+#    crc[4] orig_len[4]
+#
+# ... resulting in +18 bytes overhead of uncompressed data.
+#
+# (For more information, please refer to RFC 1951 and RFC 1952.)
+#
+# Files divided into blocks
+# 1 bit (last block flag)
+# 2 bits (block type)
+#
+# 1 block occurs every 32K -1 bytes or when there 50% compression
+# has been achieved. The smallest block type encoding is always used.
+#
+# stored:
+#    32 bits length in bytes.
+#
+# fixed:
+#    magic fixed tree.
+#    symbols.
+#
+# dynamic:
+#    dynamic tree encoding.
+#    symbols.
+#
+#
+# The buffer for decompression in place is the length of the uncompressed
+# data, plus a small amount extra to keep the algorithm safe. The
+# compressed data is placed at the end of the buffer.  The output pointer
+# is placed at the start of the buffer and the input pointer is placed
+# where the compressed data starts. Problems will occur when the output
+# pointer overruns the input pointer.
+#
+# The output pointer can only overrun the input pointer if the input
+# pointer is moving faster than the output pointer.  A condition only
+# triggered by data whose compressed form is larger than the uncompressed
+# form.
+#
+# The worst case at the block level is a growth of the compressed data
+# of 5 bytes per 32767 bytes.
+#
+# The worst case internal to a compressed block is very hard to figure.
+# The worst case can at least be bounded by having one bit that represents
+# 32764 bytes and then all of the rest of the bytes representing the very
+# very last byte.
+#
+# All of which is enough to compute an amount of extra data that is required
+# to be safe.  To avoid problems at the block level allocating 5 extra bytes
+# per 32767 bytes of data is sufficient.  To avoid problems internal to a
+# block adding an extra 32767 bytes (the worst case uncompressed block size)
+# is sufficient, to ensure that in the worst case the decompressed data for
+# block will stop the byte before the compressed data for a block begins.
+# To avoid problems with the compressed data's meta information an extra 18
+# bytes are needed.  Leading to the formula:
+#
+# extra_bytes = (uncompressed_size >> 12) + 32768 + 18
+#
+# Adding 8 bytes per 32K is a bit excessive but much easier to calculate.
+# Adding 32768 instead of 32767 just makes for round numbers.
+#
+# Above analysis is for decompressing gzip compressed kernel only. Up to
+# now 6 different decompressor are supported all together. And among them
+# xz stores data in chunks and has maximum chunk of 64K. Hence safety
+# margin should be updated to cover all decompressors so that we don't
+# need to deal with each of them separately. Please check
+# the description in lib/decompressor_xxx.c for specific information.
+#
+# extra_bytes = (uncompressed_size >> 12) + 65536 + 128
+
+#define ZO_z_extra_bytes	((ZO_z_output_len >> 12) + 65536 + 128)
+#if ZO_z_output_len > ZO_z_input_len
+# define ZO_z_extract_offset	(ZO_z_output_len + ZO_z_extra_bytes - \
+				 ZO_z_input_len)
+#else
+# define ZO_z_extract_offset	ZO_z_extra_bytes
+#endif
+
+/*
+ * The extract_offset has to be bigger than ZO head section. Otherwise when
+ * the head code is running to move ZO to the end of the buffer, it will
+ * overwrite the head code itself.
+ */
+#if (ZO__ehead - ZO_startup_32) > ZO_z_extract_offset
+# define ZO_z_min_extract_offset ((ZO__ehead - ZO_startup_32 + 4095) & ~4095)
+#else
+# define ZO_z_min_extract_offset ((ZO_z_extract_offset + 4095) & ~4095)
+#endif
+
+#define ZO_INIT_SIZE	(ZO__end - ZO_startup_32 + ZO_z_min_extract_offset)
+
 #define VO_INIT_SIZE	(VO__end - VO__text)
 #if ZO_INIT_SIZE > VO_INIT_SIZE
-#define INIT_SIZE ZO_INIT_SIZE
+# define INIT_SIZE ZO_INIT_SIZE
 #else
-#define INIT_SIZE VO_INIT_SIZE
+# define INIT_SIZE VO_INIT_SIZE
 #endif
+
 init_size:		.long INIT_SIZE		# kernel initialization size
+handover_offset:	.long 0			# Filled in by build.c
 
 # End of setup header #####################################################
 
 	.section ".entrytext", "ax"
 start_of_setup:
-#ifdef SAFE_RESET_DISK_CONTROLLER
-# Reset the disk controller.
-	movw	$0x0000, %ax		# Reset disk controller
-	movb	$0x80, %dl		# All disks
-	int	$0x13
-#endif
-
 # Force %es = %ds
 	movw	%ds, %ax
 	movw	%ax, %es
