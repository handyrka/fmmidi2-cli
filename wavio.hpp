#pragma once
#include <string>

class wav_writer
{
public:
    wav_writer();
    ~wav_writer();
    void clear();
    void write(char* chunk, int size);
    bool save(std::string filename, int sampleRate);

private:
    char* data;
    unsigned int length;
};