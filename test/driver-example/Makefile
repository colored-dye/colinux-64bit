.PHONY: all clean

include Makefile.inc

driver_name = dpp-example
objs = $(driver_name).o
target = $(driver_name).sys

all: $(target)

%.o: %.c
	$(call BUILD_C_OBJECT,$<,$@)

$(target): $(objs)
	$(call LINK_C_KERNEL_TARGET,$<,$@)

clean:
	rm -f *.o *.sys
