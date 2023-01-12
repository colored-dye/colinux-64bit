name = checksum
src = $(name).c
target = $(name).out

.PHONY: all clean
all: $(target)

$(target): $(src)
	cc -o $@ $^

clean:
	rm -f *.o $(target)

