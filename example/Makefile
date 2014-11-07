# 'make depend'   uses makedepend to automatically generate dependencies
#                   (dependencies are added to end of Makefile)
# 'make'          build executable file 'examples'
# 'make headers'  compile only HSA headers

CC := clang
CFLAGS :=  -Wall -pedantic -ansi

CPPC := clang++
CPPFLAGS := -m64 -Wall -pedantic -std=c++11 -pthread

INCLUDES := -I"../api"

SRCS := hsa.cc examples.cc

HDRS := ../api/hsa.h ../api/hsa_ext.h

OBJS := $(SRCS:.c=.o)

MAIN := examples

.PHONY: depend clean

# Compile the CPU runtime and HSA examples
all:    $(MAIN)
	$(CPPC) $(CPPFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

$(MAIN): $(OBJS)
	$(CPPC) $(CPPFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# Compile the HSA headers (C99)
headers:  $(HDRS)
	$(CC) $(CFLAGS) $(HDRS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
