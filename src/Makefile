CC = g++
CPPFLAGS = -std=c++11 -O2 -Wall -Werror -Wshadow `pkg-config gtkmm-3.0 --cflags`
LDFLAGS = `pkg-config gtkmm-3.0 --libs`
SOURCES = $(shell find . -name "*.cpp" | awk '{print length, $$0;}' | sort -rn | cut -d ' ' -f 2-)
OBJECTS = $(SOURCES:.cpp=.o)

all: main.o
	make -C ./gui/
	make -C ./core/
# clang doesn't seem to care, but gcc is VERY particular about the
# placement of linker and compiler flags (putting LDFLAGS in front
# of OBJECTS won't compile
	$(CC) $(CPPFLAGS) $(OBJECTS) $(LDFLAGS) -o spotcheck

main.o: gui/main_window.hpp
	$(CC) $(CPPFLAGS) -c main.cpp -o main.o

clean:
	find . -name "*.o" | xargs rm
