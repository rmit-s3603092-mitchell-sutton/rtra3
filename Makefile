DEBUG = -g
OPTIMISE = -O2

CFLAGS = `sdl2-config --cflags` $(DEBUG) $(OPTIMISE) -std=c99 -Wall
LDFLAGS = `sdl2-config --libs` -lGL -lGLU -lglut -lm

HEADERS = shaders.h
OBJECTS = shaders.o main.o 
EXE = main

all: $(EXE)

$(EXE): $(OBJECTS) $(HEADERS)
	g++ -o $@ $(OBJECTS) $(LDFLAGS)


shaders.o: shaders.cpp shaders.h
	g++ $(LDFLAGS) -o shaders.cpp



clean:
	rm -f $(EXE) $(OBJECTS)
