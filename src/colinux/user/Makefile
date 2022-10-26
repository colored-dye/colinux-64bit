SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

user.a: user.o
	$(AR) rc $^

user.o: $(OBJS)
	$(LD) -r -o $@ $^

%.o: %.c
	$(CC) -c $^
