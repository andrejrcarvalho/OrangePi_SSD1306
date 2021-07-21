CC=g++

# Libraries to include (if any)
LIBS=-lpthread -lwiringPi

# Compiler flags
CFLAGS=-Wall -Wextra -ggdb -pedantic -D_POSIX_C_SOURCE=200809L -O #-pg

# Linker flags
LDFLAGS=#-pg

# Indentation flags
# IFLAGS=-br -brs -brf -npsl -ce -cli4 -bli4 -nut
IFLAGS=-linux -brs -brf -br

# Name of the executable
PROGRAM=display.out


# Object files required to build the executable
PROGRAM_OBJS=main.o SSD1306.o ubuntuMono8pt.o system_info.o

# Clean and all are not files
.PHONY: clean all docs indent debugon

all: $(PROGRAM)

# activate DEBUG, defining the SHOW_DEBUG macro
debugon: CFLAGS += -D SHOW_DEBUG -g
debugon: $(PROGRAM)

# activate optimization (-O...)
OPTIMIZE_FLAGS=-O2 # possible values (for gcc): -O2 -O3 -Os -Ofast
optimize: CFLAGS += $(OPTIMIZE_FLAGS)
optimize: LDFLAGS += $(OPTIMIZE_FLAGS)
optimize: $(PROGRAM)

$(PROGRAM): $(PROGRAM_OBJS)
	$(CC) -o $@ $(PROGRAM_OBJS) $(LIBS) $(LDFLAGS)

# Dependencies
main.o: main.cpp SSD1306.h system_info.h

SSD1306.o: ubuntuMono8pt.h

#how to create an object file (.o) from C file (.c)
.cpp.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o core.* *~ $(PROGRAM) *.bak $(PROGRAM_OPT).h $(PROGRAM_OPT).c
