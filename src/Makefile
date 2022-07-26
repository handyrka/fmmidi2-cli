CXX=g++
ARGS=-lao -lvorbis -lvorbisenc -logg -Wall -static-libstdc++ -static-libgcc -g
FILES=main.cpp player.cpp fm/filter.cpp fm/midisynth.cpp fm/sequencer.cpp out/wavout.cpp out/vorbisout.cpp

.PHONY: build

build:
	$(CXX) $(FILES) $(ARGS) -o ../fmmidi.exe

clean:
	rm fmmidi.exe