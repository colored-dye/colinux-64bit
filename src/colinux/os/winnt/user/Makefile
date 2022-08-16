user-osdep.a: user-osdep.o
	$(AR) rc $@ $^

user-all.a: user-all.o
	$(AR) rc $@ $^

user-osdep.o: *.c
	$(CC) -o $@ $^

user-all.o: user-osdep.a ../../../user.a
	$(CC) -o $@ $^

.PHONY: clean
clean:
	rm -f user-osdep.a user-osdep.o user-all.o user-all.a
