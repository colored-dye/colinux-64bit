SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

build.o: $(OBJS)
	$(CROSS)$(LD) -r -o $@ $^

%.o: %.c
	$(CROSS)$(CC) -c $^ $(CFLAGS)

.PHONY: clean
clean:
	rm *.o
