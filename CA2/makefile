EXEC = ./exec
BUILDS = ./builds

all: clean createLog Main Bill Building Gas Electricity Water

Main: Main.cpp utils
	g++ Main.cpp $(BUILDS)/utils.o -o main.out

Bill: BillResource utils
	g++ $(BUILDS)/BillResource.o $(BUILDS)/utils.o -o $(EXEC)/Bill.out

Building: Building.cpp utils
	g++ Building.cpp $(BUILDS)/utils.o -o $(EXEC)/Building.out

Gas: GasResource Resource utils
	g++ $(BUILDS)/GasResource.o $(BUILDS)/Resource.o $(BUILDS)/utils.o -o $(EXEC)/Gas.out

Electricity: ElectricityResource Resource utils
	g++ $(BUILDS)/ElectricityResource.o $(BUILDS)/Resource.o $(BUILDS)/utils.o -o $(EXEC)/Electricity.out

Water: WaterResource Resource utils
	g++ $(BUILDS)/WaterResource.o $(BUILDS)/Resource.o $(BUILDS)/utils.o -o $(EXEC)/Water.out

BillResource: Bill.cpp utils.hpp
	g++ -c Bill.cpp -o $(BUILDS)/BillResource.o

ElectricityResource: Electricity.cpp Resource.hpp utils.hpp
	g++ -c Electricity.cpp -o $(BUILDS)/ElectricityResource.o

WaterResource: Water.cpp Resource.hpp utils.hpp
	g++ -c Water.cpp -o $(BUILDS)/WaterResource.o

GasResource: Gas.cpp Resource.hpp utils.hpp
	g++ -c Gas.cpp -o $(BUILDS)/GasResource.o

Resource: Resource.cpp Resource.hpp utils.hpp
	g++ -c Resource.cpp -o $(BUILDS)/Resource.o

utils: utils.cpp utils.hpp
	g++ -c utils.cpp -o $(BUILDS)/utils.o

clean:
	rm -f *.txt *.o *.out ./pipes/* ./exec/* ./builds/* 

createLog:
	touch logs.txt

cleanPipes:
	rm ./pipes/*