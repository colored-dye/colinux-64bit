SRCS_CC := $(wildcard *.c)
OBJS_CC := $(patsubst %.c, %.o, $(SRCS_CC))
SRCS_CPP := $(wildcard *.cpp)
OBJS_CPP := $(patsubst %.cpp, %.o, $(SRCS_CPP))

build.a: $(OBJS_CC) $(OBJS_CPP)
	$(CROSS)$(AR) rc $^

%.o: %.c
	$(CROSS)$(CC) -c $^ $(CFLAGS)

%.o: %.cpp
	$(CROSS)$(CXX) -c $^ $(CFLAGS)
