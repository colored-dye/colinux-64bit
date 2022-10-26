SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))

user-osdep.a: user-osdep.o
	$(CROSS)$(AR) rc $@ $^

user-all.a: user-osdep.a ../../../user.a
	$(CROSS)$(AR) rc $@ $^

user-osdep.o: $(OBJS)
	$(CROSS)$(LD) -r -o $@ $^

.PHONY: clean
clean:
	rm *.o *.a
