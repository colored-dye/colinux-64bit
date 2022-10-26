SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

build.o: $(OBJS)
	$(LD) -r -o $@ $^

%.o: %.c
	$(CC) -c $^
