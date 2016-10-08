CC = g++
CPPFLAGS = -std=c++11 -O2
SOURCES = $(shell find . -name "*.cpp" | awk '{print length, $$0;}' | sort -rn | cut -d ' ' -f 2-)
OBJECTS = $(SOURCES:.cpp=.o)

all: $(OBJECTS)

.cpp.o:
	$(CC) $(CPPFLAGS) -c $< -o $@

clean:
	rm *.o

# MANUAL LIST OF DEPENDENCIES, MAKEDEPEND DOESN'T LIKE CAIRO HEADERS
./work_queue.o: work_queue.hpp smart_thread.hpp
./parse_tiff.o: parse_tiff.hpp option.hpp
./parse_gal.o: parse_gal.hpp option.hpp
