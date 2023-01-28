NAME = console
SRCS = main.cpp
TARGET = $(NAME).exe

PREFIX = $(abspath ../../mingw/i686)

Q = $(PREFIX)/bin/i686-w64-mingw32-
CC = gcc

CFLAGS = -lfltk

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(SRCS)
	$(Q)$(CC) -o $@ $^

clean:
	rm $(TARGET)
