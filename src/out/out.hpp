/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */ 
#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <fstream>

class file_output
{
public:
    inline virtual bool open(std::string filename, int sampleRate) { return false;}
    inline virtual void write(char* chunk, int size) {}
    inline virtual void close() { }

protected:
    uint32_t length = 0;
};
