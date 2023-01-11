name = checksum
src = $(name).c
target = $(name)

.PHONY: all clean
all: $(target)

$(target): $(src)
	cc -o $@ $^

clean:
	rm -f *.o $(target)

