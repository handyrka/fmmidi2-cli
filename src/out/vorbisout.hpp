/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */ 
#pragma once
#include "out.hpp"
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <fstream>

class vorbis_output : public file_output
{
public:
    float quality = 0.5; //setting quality

    bool open(std::string filename, int sampleRate) override;
    void write(char* chunk, int size) override;
    void close() override;
    void analyze();

private:
    ogg_stream_state os;
    ogg_packet op;
    ogg_page og;

    vorbis_comment vc;
    vorbis_info vi;
    vorbis_dsp_state vd;
    vorbis_block vb;

    std::fstream stream;
};
