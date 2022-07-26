/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */ 
#include "player.hpp"
#include "midisynth.hpp"
#include "program.hpp"
#include "sequencer.hpp"
#include <ao/ao.h>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>

class fmOutSynth : public fmOut
{
	private:
		midisynth::synthesizer *synth;
		midisynth::fm_note_factory *note_factory;
		midisynth::drum_parameter p;

	public:
		fmOutSynth()
		{
			note_factory = new midisynth::fm_note_factory;
			synth = new midisynth::synthesizer(note_factory);
			
			//Loading programs from header
        	load_program(note_factory);
		}
	
		void midi_message(int port, uint_least32_t message)
		{
			synth->midi_event(message);
		}
		
		void sysex_message(int port, const void* data, std::size_t size)
		{
			synth->sysex_message(data, size);
		}
		
		void meta_event(int type, const void* data, std::size_t size)
		{
			if(type == META_EVENT_ALL_NOTE_OFF)
				synth->all_note_off();
		}
		
		int synthesize(int_least16_t* output, std::size_t samples, float rate)
		{
			return synth->synthesize(output, samples, rate);
		}
		
		void set_mode(midisynth::system_mode_t mode)
		{
			synth->set_system_mode(mode);
		}
		
		int get_main_volume()
		{
			return synth->get_main_volume();
		}

		void set_main_volume(int volume)
		{
			synth->set_main_volume(volume);
		}
		
		void reset()
		{
			synth->reset();
		}
};

bool midi_player::init(int playback_rate)
{	
	audio_init();

	//Creating new Sequencer and Output FM Synth
	out = new fmOutSynth();
	seq = new midisequencer::sequencer();

	//Allocating chunks
	rate = playback_rate;
	
	sampleSize = rate * delta * 2 * sizeof(short);
	sampleOut = (short*)malloc(SAMPOUT_SIZE);
	memset(sampleOut, 0, SAMPOUT_SIZE);

	blankOut = (short*)malloc(rate * 2 * sizeof(short));
	memset(blankOut, 0, rate * 2 * sizeof(short));

	//We don't need to initilize audio device for custom wav output
	if(outToFile) return true;

	if(aoDriver == -1)
	{
	    std::cerr << "[midi_player::init()]: Failed to initialize audio driver" << std::endl;
		return false;
	}
	
	ao_sample_format smpformat;
	smpformat.bits = 16;
	smpformat.rate = rate;
	smpformat.channels = 2;
	smpformat.byte_format = AO_FMT_NATIVE;
	smpformat.matrix = NULL;

	aoDevice = ao_open_live(aoDriver, &smpformat, NULL);
	if(!aoDevice)
	{
	    std::cerr << "[midi_player::init()]: Failed to initialize live audio driver: " << std::strerror(errno) << std::endl;
	    return false;
	}

	return true;
}

bool midi_player::play()
{
	std::string filename = playlist.at(playlistIndex);
	FILE *midi = fopen(filename.c_str(), "rb");
	
	//If file is failed to open
	if(!midi)
	{
    	std::cerr << "[midi_player::play()]: Failed to open a midi file." << std::endl;
		return false;
	}

	//Trying to load a midi sequence
	if(!seq->load(midi))
	{
    	std::cerr << "[midi_player::play()]: Failed to load midi sequence." << std::endl;	
		fclose(midi);
		return false;
	}
	seq->rewind();

	fclose(midi);

	//Show song information
	printf("\n[%s] (%lu/%lu)\nCopyright: %s\n",filename.c_str(), playlistIndex + 1, playlist.size(), seq->get_copyright().c_str());
	
play:
	time = 0;
	totalTime = seq->get_total_time();
	out->set_mode(mode);

	bool buffered = false;
	while(time < totalTime)
	{		
		seq->play(time, out);
		time += delta;

		out->synthesize(sampleOut, rate * delta, rate);
		if(!buffered)
		{
			for(int i = 0; i < 8; i++)
				audio_play_chunk(blankOut, sampleSize);
		}
		audio_play_chunk(sampleOut, sampleSize);
		buffered = true;

		// UI
		if((int)(time * 10) % 10 == 0)
			ui_update();
	}

	//Write to file when done
	if(outToFile)
	{
		wav.save(outFilename.c_str(), rate);
		return true;
	}

	if(playlistIndex < playlist.size() - 1)
	{
		playlistIndex++;
		return play();
	}
	
	if(loop)
	{
		if(playlistIndex >= playlist.size() - 1)
		{
			playlistIndex = 0;
			return play();
		}
		goto play;
	}

	return true;
}

void midi_player::close()
{
	audio_close();
}

void midi_player::setOutput(std::string filename)
{
	outFilename = filename;
	outToFile = true;
}

void midi_player::setMode(midisynth::system_mode_t mode)
{
	this->mode = mode;
}

void midi_player::setLoop(bool loop)
{
	this->loop = loop;
}

void midi_player::add(std::string filename)
{
	playlist.push_back(filename);
}

void midi_player::ui_update()
{
	//Free space for progress bar + time
	for(int i = 0; i < 38; i++)
		printf(" ");
	
	printf("\r"); //Return
	printf("%02d:%02d [", (int)time / 60, (int)time % 60);

	//Draw progress bar
	for(int i = 0; i < 15; i++)
		if(i < (time / totalTime) * 15)
			printf("#");
		else
		 	printf(".");

	printf("] %02d:%02d\r", (int)totalTime / 60, (int)totalTime % 60);
	fflush(stdout);
}

void midi_player::audio_init()
{
	if(outToFile) return; // We use custom output to file

	ao_initialize();
	atexit(ao_shutdown);
	aoDriver = ao_default_driver_id();
}

void midi_player::audio_close()
{
	if(outToFile) return;

	ao_close(aoDevice);
}

void midi_player::audio_play_chunk(short* chunk, int size)
{
	assert(size >= 0);

	if(outToFile)
	{
		wav.write((char*)chunk, size);
		return;
	}

	ao_play(aoDevice, (char*)chunk, size);
}