build.a: build.o
	$(CROSS)$(AR) -rc $^

build.o: daemon.o tap-win32.o
	$(CROSS)$(LD) -r -o $@ $^

daemon.o: daemon.cpp
	$(CROSS)$(CXX) -c $^ $(CFLAGS)

tap-win32.o: tap-win32.c
	$(CROSS)$(CC) -c $^ $(CFLAGS)

.PHONY: clean
clean:
	rm *.o *.a
