/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */ 
#include "wavio.hpp"
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <cstring>

typedef struct __wavheader
{
  /* RIFF Chunk Descriptor */
  uint8_t RIFF[4] = {'R', 'I', 'F', 'F'}; // RIFF Header Magic header
  uint32_t chunkSize;                     // RIFF Chunk Size
  uint8_t WAVE[4] = {'W', 'A', 'V', 'E'}; // WAVE Header
  
  /* "fmt" sub-chunk */
  uint8_t fmt[4] = {'f', 'm', 't', ' '}; // FMT header
  uint32_t subchunkSize = 16;           // Size of the fmt chunk
  uint16_t audioFmt = 1;                // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM
                                        // Mu-Law, 258=IBM A-Law, 259=ADPCM
  uint16_t numberOfChannels = 2;        // Number of channels 1=Mono 2=Sterio
  uint32_t samplesPerSecond = 16000;    // Sampling Frequency in Hz
  uint32_t bytesPerSec = 16000 * 2;     // bytes per second
  uint16_t blockAlign = 4;              // 2=16-bit mono, 4=16-bit stereo
  uint16_t bitsPerSample = 16;          // Number of bits per sample
  
  /* "data" sub-chunk */
  uint8_t subchunk2ID[4] = {'d', 'a', 't', 'a'}; // "data"  string
  uint32_t subchunk2Size;                        // Sampled data length
} wav_header;

wav_writer::wav_writer()
{
    clear();
}

void wav_writer::clear()
{
    if(data != nullptr)
        free(data);
    data = (char*)malloc(1);
    length = 0;
}

void wav_writer::write(char *chunk, int size)
{
    data = (char*)realloc((void*)data, length + size);
    memcpy(data + length, chunk, size);
    length += size;   
}

bool wav_writer::save(std::string filename, int sampleRate)
{
    std::ofstream stream(filename, std::ios::binary | std::ios::out);

    if(!stream)
        return false;

    wav_header header;
    header.chunkSize = length + sizeof(wav_header) - 8;
    header.subchunk2Size = length;
    header.samplesPerSecond = sampleRate;
    header.bytesPerSec = sampleRate * 4;

    stream.write(reinterpret_cast<char *>(&header), sizeof(header));
    stream.write((const char*)data, length);
    stream.close();

    free(data);

    return true;
}
