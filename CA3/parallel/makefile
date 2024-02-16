BUILDS = ./builds

all: clean Main

Main: main.cpp ParallelFilter Photo
	g++ -pthread main.cpp $(BUILDS)/ParallelFilter.o $(BUILDS)/Photo.o -o ImageFilters.out

ParallelFilter: ParallelFilter.cpp ParallelFilter.hpp Photo
	g++ -pthread -c ParallelFilter.cpp $(BUILDS)/Photo.o -o $(BUILDS)/ParallelFilter.o

Photo: Photo.cpp Photo.hpp
	g++ -c Photo.cpp -o $(BUILDS)/Photo.o

clean:
	rm -f *.o *.out output.bmp ./builds/* ./output.bmp