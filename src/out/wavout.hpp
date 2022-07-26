/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */ 
#pragma once
#include <fstream>
#include <string>
#include "out.hpp"

class wav_output : public file_output
{
public:
    bool open(std::string filename, int sampleRate) override;
    void write(char* chunk, int size) override;
    void close() override;

private:
    std::fstream stream;
    int sampleRate = 44100;
};