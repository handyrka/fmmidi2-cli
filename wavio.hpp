/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */ 
#pragma once
#include <string>

class wav_writer
{
public:
    wav_writer();
    void clear();
    void write(char* chunk, int size);
    bool save(std::string filename, int sampleRate);

private:
    char* data = nullptr;
    unsigned int length;
};