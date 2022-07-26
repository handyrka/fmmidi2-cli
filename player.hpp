/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */ 
#pragma once
#include <ao/ao.h>
#include <vector>
#include "midisynth.hpp"
#include "sequencer.hpp"
#include "wavio.hpp"

class fmOut : public midisequencer::output
{
	public:
		virtual int synthesize(int_least16_t* output, std::size_t samples, float rate) = 0;
		virtual void set_mode(midisynth::system_mode_t mode) = 0;
		virtual int get_main_volume() = 0;
		virtual void set_main_volume(int volume) = 0;
		virtual void reset() = 0;
};

class midi_player
{
public:
    bool init(int playback_rate);
    bool play();
    void close();

    void add(std::string filename);
    void setLoop(bool loop);
    void setOutput(std::string filename);
    void setMode(midisynth::system_mode_t mode);

private:
    //Synth components
    fmOut *out = NULL;
    midisequencer::sequencer *seq;
    ao_device* aoDevice;
    int aoDriver = 0;
    wav_writer wav;
    midisynth::system_mode_t mode = midisynth::system_mode_gm;
    
    //Playback
    std::vector<std::string> playlist = std::vector<std::string>();
    std::string outFilename = "";
    bool loop = false; // no loop
    bool outToFile = false;
    double time, totalTime;
    unsigned long playlistIndex = 0;

    //Fm variables
    double delta = 0.03125;
    int rate = 44100;
    int sampleSize = 0;
    #define SAMPOUT_SIZE (2 * sampleSize)

    //Chunks
    short* sampleOut, *blankOut;

    void audio_init();
    void audio_play_chunk(short* chunk, int size);
    void audio_close();
};