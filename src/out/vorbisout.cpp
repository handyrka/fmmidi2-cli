/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */ 
#include "vorbisout.hpp"
#include <cstdint>
#include <ctime>

bool vorbis_output::open(std::string filename, int sampleRate)
{
    stream.open(filename, std::ios::binary | std::ios::out);

    if(!stream.is_open())
        return false;

    vorbis_info_init(&vi);

    if(vorbis_encode_init_vbr(&vi,2,sampleRate,quality))
        return false;

    vorbis_comment_init(&vc);
    vorbis_comment_add_tag(&vc,"ENCODER","fmmidi2");

    vorbis_analysis_init(&vd,&vi);
    vorbis_block_init(&vd,&vb);

    srand(time(NULL));
    ogg_stream_init(&os,rand());

    //Writing header
    {
        ogg_packet header;
        ogg_packet header_comm;
        ogg_packet header_code;

        vorbis_analysis_headerout(&vd,&vc,&header,&header_comm,&header_code);
        ogg_stream_packetin(&os,&header);

        ogg_stream_packetin(&os,&header_comm);
        ogg_stream_packetin(&os,&header_code);

        if(!ogg_stream_flush(&os, &og))
            return false;

        stream.write((const char*)og.header, og.header_len);
        stream.write((const char*)og.body, og.body_len);
    }

    return true;
}

void vorbis_output::write(char *chunk, int size)
{
    //Encoding
    float** buffer = vorbis_analysis_buffer(&vd, size / 4);

    int j;
    for(j = 0; j < size / 4; j++)
    {
        buffer[0][j] = ((chunk[j * 4 + 1] << 8) | (0x00ff & (int) chunk[j * 4])) / 32768.f;
        buffer[1][j] = ((chunk[j * 4 + 3] << 8) | (0x00ff & (int) chunk[j * 4 + 2])) / 32768.f;
    }
    
    vorbis_analysis_wrote(&vd, j);
    analyze();
}

void vorbis_output::close()
{
    vorbis_analysis_wrote(&vd, 0);
    analyze();

    ogg_stream_clear(&os);
    vorbis_block_clear(&vb);
    vorbis_dsp_clear(&vd);
    vorbis_comment_clear(&vc);
    vorbis_info_clear(&vi);
    
    stream.close();
}

void vorbis_output::analyze()
{
    //Analyze
    while(vorbis_analysis_blockout(&vd, &vb) == 1)
    {
        vorbis_analysis(&vb, nullptr);
        vorbis_bitrate_addblock(&vb);

        while(vorbis_bitrate_flushpacket(&vd, &op))
        {
            ogg_stream_packetin(&os, &op);

            bool eos = false;
            while(!eos)
            {
                if(ogg_stream_pageout(&os, &og) == 0)
                    break;

                stream.write((const char*)og.header, og.header_len);
                stream.write((const char*)og.body, og.body_len);

                if(ogg_page_eos(&og) > 0)
                    eos = true;
            }
        }
    }
}