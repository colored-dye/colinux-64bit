COMMON_TARGETS := queue.o \
				console.o \
				debug.o \
				errors.o \
				messages.o \
				module_repr.o \
				libc.o \
				snprintf.o \
				unicode.o

common.a: $(COMMON_TARGETS)
	$(CC) -o $@ $^
