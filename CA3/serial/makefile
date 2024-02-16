BUILDS = ./builds

all: clean Main

Main: main.cpp Filter Photo
	g++ -pthread main.cpp $(BUILDS)/Filter.o $(BUILDS)/Photo.o -o ImageFilters.out

Filter: Filter.cpp Filter.hpp Photo
	g++ -pthread -c Filter.cpp $(BUILDS)/Photo.o -o $(BUILDS)/Filter.o

Photo: Photo.cpp Photo.hpp
	g++ -c Photo.cpp -o $(BUILDS)/Photo.o

clean:
	rm -f *.o *.out output.bmp ./builds/* ./output.bmp