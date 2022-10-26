SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

build.a: build.o
	$(AR) rc $^

build.o: $(OBJS)
	$(LD) -r -o $@ $^

%.o: %.cpp
	$(CXX) -c $^
