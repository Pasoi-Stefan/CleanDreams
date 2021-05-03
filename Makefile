run: build
	./cleandreams_washing_machine

build: cleandreams_washing_machine.cpp
	g++ cleandreams_washing_machine.cpp -o cleandreams_washing_machine -std=c++17 -lpistache 
