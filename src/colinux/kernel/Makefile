TARGETS := block.o \
			debug.o \
			fileblock.o \
			filesystem.o \
			manager.o \
			monitor.o \
			pages.o \
			pci.o \
			reversedpfns.o \
			scsi.o \
			transfer.o \
			video.o

%.o: %.c
	$(CROSS)$(CC) -c $^ $(CFLAGS)

build.o: $(TARGETS)
	$(CROSS)$(CC) -o $@ $^ $(CFLAGS)
