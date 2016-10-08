CC = g++
CPPFLAGS = -std=c++11 -O2 `pkg-config gtkmm-3.0 --cflags`
LDFLAGS = `pkg-config gtkmm-3.0 --libs`
SOURCES = $(shell find . -name "*.cpp" | awk '{print length, $$0;}' | sort -rn | cut -d ' ' -f 2-)
OBJECTS = $(SOURCES:.cpp=.o)

all: main.o
	make -C ./gui/
	make -C ./core/
	$(CC) $(CPPFLAGS) $(OBJECTS) $(LDFLAGS) -o analyzer

main.o: gui/main_window.hpp
	$(CC) $(CPPFLAGS) -c main.cpp -o main.o

clean:
	find . -name "*.o" | xargs rm
