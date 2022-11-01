--- linux-2.6.33.7-source/arch/x86/boot/compressed/head_32.S	2022-08-08 11:45:13.625120100 +0800
+++ linux-4.9.325-source/arch/x86/boot/compressed/head_32.S	2022-11-01 13:41:06.989120200 +0800
@@ -29,15 +29,110 @@
 #include <asm/page_types.h>
 #include <asm/boot.h>
 #include <asm/asm-offsets.h>
+#include <asm/bootparam.h>
+
+/*
+ * The 32-bit x86 assembler in binutils 2.26 will generate R_386_GOT32X
+ * relocation to get the symbol address in PIC.  When the compressed x86
+ * kernel isn't built as PIC, the linker optimizes R_386_GOT32X
+ * relocations to their fixed symbol addresses.  However, when the
+ * compressed x86 kernel is loaded at a different address, it leads
+ * to the following load failure:
+ *
+ *   Failed to allocate space for phdrs
+ *
+ * during the decompression stage.
+ *
+ * If the compressed x86 kernel is relocatable at run-time, it should be
+ * compiled with -fPIE, instead of -fPIC, if possible and should be built as
+ * Position Independent Executable (PIE) so that linker won't optimize
+ * R_386_GOT32X relocation to its fixed symbol address.  Older
+ * linkers generate R_386_32 relocations against locally defined symbols,
+ * _bss, _ebss, _got, _egot and _end, in PIE.  It isn't wrong, just less
+ * optimal than R_386_RELATIVE.  But the x86 kernel fails to properly handle
+ * R_386_32 relocations when relocating the kernel.  To generate
+ * R_386_RELATIVE relocations, we mark _bss, _ebss, _got, _egot and _end as
+ * hidden:
+ */
+	.hidden _bss
+	.hidden _ebss
+	.hidden _got
+	.hidden _egot
+	.hidden _end
 
 	__HEAD
 ENTRY(startup_32)
+#ifdef CONFIG_EFI_STUB
+	jmp	preferred_addr
+
+	/*
+	 * We don't need the return address, so set up the stack so
+	 * efi_main() can find its arguments.
+	 */
+ENTRY(efi_pe_entry)
+	add	$0x4, %esp
+
+	call	1f
+1:	popl	%esi
+	subl	$1b, %esi
+
+	popl	%ecx
+	movl	%ecx, efi32_config(%esi)	/* Handle */
+	popl	%ecx
+	movl	%ecx, efi32_config+8(%esi)	/* EFI System table pointer */
+
+	/* Relocate efi_config->call() */
+	leal	efi32_config(%esi), %eax
+	add	%esi, 32(%eax)
+	pushl	%eax
+
+	call	make_boot_params
+	cmpl	$0, %eax
+	je	fail
+	movl	%esi, BP_code32_start(%eax)
+	popl	%ecx
+	pushl	%eax
+	pushl	%ecx
+	jmp	2f		/* Skip efi_config initialization */
+
+ENTRY(efi32_stub_entry)
+	add	$0x4, %esp
+	popl	%ecx
+	popl	%edx
+
+	call	1f
+1:	popl	%esi
+	subl	$1b, %esi
+
+	movl	%ecx, efi32_config(%esi)	/* Handle */
+	movl	%edx, efi32_config+8(%esi)	/* EFI System table pointer */
+
+	/* Relocate efi_config->call() */
+	leal	efi32_config(%esi), %eax
+	add	%esi, 32(%eax)
+	pushl	%eax
+2:
+	call	efi_main
+	cmpl	$0, %eax
+	movl	%eax, %esi
+	jne	2f
+fail:
+	/* EFI init failed, so hang. */
+	hlt
+	jmp	fail
+2:
+	movl	BP_code32_start(%esi), %eax
+	leal	preferred_addr(%eax), %eax
+	jmp	*%eax
+
+preferred_addr:
+#endif
 	cld
 	/*
 	 * Test KEEP_SEGMENTS flag to see if the bootloader is asking
 	 * us to not reload segments
 	 */
-	testb	$(1<<6), BP_loadflags(%esi)
+	testb	$KEEP_SEGMENTS, BP_loadflags(%esi)
 	jnz	1f
 
 	cli
@@ -75,12 +170,16 @@
 	addl    %eax, %ebx
 	notl	%eax
 	andl    %eax, %ebx
-#else
-	movl	$LOAD_PHYSICAL_ADDR, %ebx
+	cmpl	$LOAD_PHYSICAL_ADDR, %ebx
+	jae	1f
 #endif
+	movl	$LOAD_PHYSICAL_ADDR, %ebx
+1:
 
 	/* Target address to relocate to for decompression */
-	addl	$z_extract_offset, %ebx
+	movl    BP_init_size(%esi), %eax
+	subl    $_end, %eax
+	addl    %eax, %ebx
 
 	/* Set up the stack */
 	leal	boot_stack_end(%ebx), %esp
@@ -124,53 +223,55 @@
 	rep	stosl
 
 /*
- * Do the decompression, and jump to the new kernel..
+ * Adjust our own GOT
  */
-	leal	z_extract_offset_negative(%ebx), %ebp
-				/* push arguments for decompress_kernel: */
+	leal	_got(%ebx), %edx
+	leal	_egot(%ebx), %ecx
+1:
+	cmpl	%ecx, %edx
+	jae	2f
+	addl	%ebx, (%edx)
+	addl	$4, %edx
+	jmp	1b
+2:
+
+/*
+ * Do the extraction, and jump to the new kernel..
+ */
+				/* push arguments for extract_kernel: */
+	pushl	$z_output_len	/* decompressed length, end of relocs */
+
+	movl    BP_init_size(%esi), %eax
+	subl    $_end, %eax
+	movl    %ebx, %ebp
+	subl    %eax, %ebp
 	pushl	%ebp		/* output address */
+
 	pushl	$z_input_len	/* input_len */
 	leal	input_data(%ebx), %eax
 	pushl	%eax		/* input_data */
 	leal	boot_heap(%ebx), %eax
 	pushl	%eax		/* heap area */
 	pushl	%esi		/* real mode pointer */
-	call	decompress_kernel
-	addl	$20, %esp
+	call	extract_kernel	/* returns kernel location in %eax */
+	addl	$24, %esp
 
-#if CONFIG_RELOCATABLE
 /*
- * Find the address of the relocations.
- */
-	leal	z_output_len(%ebp), %edi
-
-/*
- * Calculate the delta between where vmlinux was compiled to run
- * and where it was actually loaded.
- */
-	movl	%ebp, %ebx
-	subl	$LOAD_PHYSICAL_ADDR, %ebx
-	jz	2f	/* Nothing to be done if loaded at compiled addr. */
-/*
- * Process relocations.
+ * Jump to the extracted kernel.
  */
+	xorl	%ebx, %ebx
+	jmp	*%eax
 
-1:	subl	$4, %edi
-	movl	(%edi), %ecx
-	testl	%ecx, %ecx
-	jz	2f
-	addl	%ebx, -__PAGE_OFFSET(%ebx, %ecx)
-	jmp	1b
-2:
+#ifdef CONFIG_EFI_STUB
+	.data
+efi32_config:
+	.fill 4,8,0
+	.long efi_call_phys
+	.long 0
+	.byte 0
 #endif
 
 /*
- * Jump to the decompressed kernel.
- */
-	xorl	%ebx, %ebx
-	jmp	*%ebp
-
-/*
  * Stack and heap for uncompression
  */
 	.bss
