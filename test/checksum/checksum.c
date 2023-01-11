#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "checksum.h"

#define WORD_MASK (0xffffUL)
#define DWORD_MASK (0xffffffffUL)

ULONG get_checksum_offset(const UCHAR *buf) {
	ULONG checksum_offset = 0;

	IMAGE_DOS_HEADER dos_header;
	memcpy(&dos_header, buf, sizeof(dos_header));
	ULONG PE = dos_header.e_lfanew;

	IMAGE_NT_HEADERS nt_header;
	checksum_offset = PE;
	checksum_offset += (void*)&nt_header.OptionalHeader - (void*)&nt_header;
	checksum_offset += (void*)&nt_header.OptionalHeader.CheckSum - (void*)&nt_header.OptionalHeader;

	return checksum_offset;
}

ULONG calc_checksum(const UCHAR *buf, const ULONGLONG file_size, const ULONG checksum_offset) {
	ULONGLONG checksum = 0;
	const ULONGLONG TOP = 0x100000000UL;
	UCHAR *p = (UCHAR*)buf;

	for (ULONGLONG i=0; i < file_size; i += 4) {
		if (i == checksum_offset) {
			p += 4;
			continue;
		}
		ULONG dword = *(ULONG*)(p);
		p += 4;
		checksum = (checksum & DWORD_MASK) + dword + (checksum >> 32);
		if (checksum > TOP) {
			// printf("0x%016lx\n", checksum);
			checksum = (checksum & DWORD_MASK) + (checksum >> 32);
		}
	}

	checksum = (checksum & WORD_MASK) + (checksum >> 16);
	checksum += checksum >> 16;
	checksum &= WORD_MASK;
	checksum += file_size;
	checksum &= DWORD_MASK;

	return (ULONG)checksum;
}

void set_checksum(FILE *fp, const ULONG offset, const ULONG checksum) {
	fseek(fp, offset, SEEK_SET);
	fwrite(&checksum, sizeof(checksum), 1, fp);
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		fprintf(stderr, "usage: checksum <file_name>\n");
		return 1;
	}

	FILE *fp = fopen(argv[1], "rb+");
	UCHAR *buf;

	struct stat file_stat;
	stat(argv[1], &file_stat);
	ULONGLONG file_size = file_stat.st_size;
	printf("File size: 0x%lx\n", file_size);

	buf = (UCHAR*)malloc(file_size);
	fread(buf, file_size, 1, fp);

	ULONG checksum_offset = get_checksum_offset(buf);
	printf("Checksum offset: 0x%08x\n", checksum_offset);

	ULONG checksum_old = *(ULONG*)(buf + checksum_offset);
	printf("Original checksum: 0x%08x\n", checksum_old);

	ULONG checksum_new = calc_checksum(buf, file_size, checksum_offset);
	printf("Calculated checksum: 0x%08x\n", checksum_new);

	if (checksum_old == checksum_new) {
		printf("Checksum correct.\n");
	} else {
		printf("Checksum wrong. Overwrite with correct checksum.\n");
		set_checksum(fp, checksum_offset, checksum_new);
	}

	free(buf);
	fclose(fp);

	return 0;
}

