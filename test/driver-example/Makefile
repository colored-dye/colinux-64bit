.PHONY: all
.DEFAULT_GOAL = all

include Makefile.inc

driver_name = dpp-example
objs = $(driver_name).o
target = $(driver_name).sys

all: $(target)

%.o: %.c
	$(call BUILD_C_OBJECT,$<,$@)

$(target):
	$(call LINK_C_KERNEL_OBJECT,$<,$@)

clean:
	rm *.o *.sys
