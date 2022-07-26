CXX=g++
ARGS=-lao -Wall -static-libstdc++ -static-libgcc -g
FILES=main.cpp sequencer.cpp midisynth.cpp filter.cpp player.cpp wavio.cpp

.PHONY: build

build:
	$(CXX) $(FILES) $(ARGS) -o fmmidi.exe

clean:
	rm fmmidi.exe