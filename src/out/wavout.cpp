/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */ 
#include "wavout.hpp"
#include <fstream>
#include <iostream>

typedef struct __wavheader
{
    /* RIFF Chunk Descriptor */
    uint8_t RIFF[4] = {'R', 'I', 'F', 'F'};
    uint32_t chunkSize;
    uint8_t WAVE[4] = {'W', 'A', 'V', 'E'};
    
    /* "fmt" sub-chunk */
    uint8_t fmt[4] = {'f', 'm', 't', ' '};
    uint32_t subchunkSize = 16;
    uint16_t audioFmt = 1;
    uint16_t numberOfChannels = 2;
    uint32_t samplesPerSecond = 44100;    // in Hz
    uint32_t bytesPerSec = 44100 * 4;     // bytes per second
    uint16_t blockAlign = 4;              // 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample = 16;
    
    /* "data" sub-chunk */
    uint8_t subchunk2ID[4] = {'d', 'a', 't', 'a'};
    uint32_t subchunk2Size;
} wav_header;

bool wav_output::open(std::string filename, int sampleRate)
{
    stream.open(filename, std::ios::binary | std::ios::out);

    if(!stream.is_open())
        return false;

    wav_header header;
    header.samplesPerSecond = sampleRate;
    header.bytesPerSec = sampleRate * 4;
    stream.write(reinterpret_cast<char *>(&header), sizeof(header));
    
    this->sampleRate = sampleRate;
    return true;
}

void wav_output::write(char* chunk, int size)
{
    stream.write((const char*)chunk, size);
}

void wav_output::close()
{
    //Set new chunk size
    stream.seekp(4, std::ios_base::beg);
    stream << (uint32_t)(length + sizeof(wav_header) - 8);
    
    //Set new sub chunk 2 size
    stream.seekp(sizeof(wav_header) - 4, std::ios_base::beg);
    stream << (uint32_t)(length + sizeof(wav_header) - 44);
    
    
    //Write new header
    stream.close();
}
