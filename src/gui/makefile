CC = g++
CPPFLAGS = -std=c++11 -O2 -Wall -Werror -Wshadow `pkg-config gtkmm-3.0 --cflags`
SOURCES = $(shell find . -name "*.cpp" | awk '{print length, $$0;}' | sort -rn | cut -d ' ' -f 2-)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS)

.cpp.o:
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm *.o

# MANUAL LIST OF DEPENDENCIES, MAKEDEPEND DOESN'T LIKE CAIRO HEADERS
./main_window.o: main_window.hpp ../core/work_queue.hpp ../core/option.hpp
./main_window.o: ../core/parse_tiff.hpp ../core/parse_gal.hpp ../core/analyze_unit.hpp
./main.o: main_window.hpp
./console.o: console.hpp
