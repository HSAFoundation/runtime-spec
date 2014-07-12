# Makefile junk

CXX := clang++

CXXFLAGS := -Wall -g -std=c++11

INCLUDES := -I"../api"

SRCS = simple_dispatch.c hsa.cc

OBJS = $(SRCS:.c=.o)
OBJS = $(SRCS:.cc=.o)

# define the executable file
MAIN := simple_dispatch

.PHONY: depend clean

all:	$(MAIN)

$(MAIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS)

.c.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

.cc.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
