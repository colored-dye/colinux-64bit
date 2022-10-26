SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

daemon.o: $(OBJS)
	$(CROSS)$(LD) -r -o $@ $^

%.o: %.c
	$(CROSS)$(CC) -c $^ $(CFLAGS)
