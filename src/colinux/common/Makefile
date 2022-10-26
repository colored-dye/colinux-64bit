SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

%.o: %.c
	$(CROSS)$(CC) -c $^ $(CFLAGS)

common.a: $(OBJS)
	$(CROSS)$(AR) -r $@ $^
