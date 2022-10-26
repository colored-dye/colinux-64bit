SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

build.o: $(OBJS) lowlevel/build.o

%.o: %.c
	$(CROSS)$(CC) -c $^ $(CFLAGS)

lowlevel/build.o:
	make -C lowlevel

.PHONY: clean
	rm *.o
