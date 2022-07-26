/* Copyright (c) 2003-2005 yuno
 * Copyright (c) 2015-2018 supercatexpert 
 * Copyright (c) 2022 Roman Savchenko 
 */

#include "fm/midisynth.hpp"
#include "out/vorbisout.hpp"
#include "out/wavout.hpp"
#include "player.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#define FMMIDI_VERSION "2.0.5"
#define assert_and_help(x) if(!(x)) { show_help(); exit(1); }

void show_help()
{
	std::cout << "fmmidi v" FMMIDI_VERSION " [files...] [options...]" << std::endl;
	std::cout << "Authors: yuno, supercatexpert, Roman Savchenko" << std::endl;
	std::cout << "https://github.com/handyrka/fmmidi2" << std::endl << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "-h: Show help screen." << std::endl;
	std::cout << "-l: Repeat playback indefinitely." << std::endl;
	std::cout << "-oW [filename]: Output to a WAV file." << std::endl;
	std::cout << "-oV [filename]: Output to a OGG vorbis file." << std::endl;
	std::cout << "-q [-1 to 10]: Set output quality. (OGG vorbis only)" << std::endl;
	std::cout << "-s [number]: Set sample rate." << std::endl;
	std::cout << "-m [gm/gm2/gs/xg]: Forces system mode." << std::endl << std::endl;
}

int main(int argc, char *argv[])
{
	midi_player player;
	int playbackRate = 44100;

	assert_and_help(argc >= 2);

	for(int i = 1; i < argc; i++)
	{
		std::string arg (argv[i]);
		auto index = arg.find("-");
		
		if(index != std::string::npos)
		{
			char c = arg.at(index + 1);
			
			switch (c) 
			{
				case 'h':
				{
					show_help();
					return EXIT_FAILURE;
				}
				break;

				case 'l':
				{
					player.setLoop(true);
				}
				break;

				case 'o':
				{
					assert_and_help(index + 2 < arg.length());
					assert_and_help(i + 1 < argc);

					std::string file = argv[i + 1];
					player.setOutput(file);

					switch (arg.at(index + 2)) 
					{
						case 'W':
							player.fileout = new wav_output();
							break;
						case 'V':
							player.fileout = new vorbis_output();
							break;
						default:
							show_help();
							exit(EXIT_FAILURE);
							break;
					}

					i++;
				}
				break;

				case 'q':
				{
					assert_and_help(i + 1 < argc);
					vorbis_output* vrb = dynamic_cast<vorbis_output*>(player.fileout);
					
					if(!vrb)
					{
						std::cerr << "Quality option is only supported in OGG vorbis output mode." << std::endl;
						return EXIT_FAILURE;
					}

					vrb->quality = atoi(argv[i + 1]) / 10.0f;
				}
				break;

				case 's':
				{
					assert_and_help(i + 1 < argc);
					playbackRate = atoi(argv[i + 1]);
					i++;
				}
				break;

				case 'm':
				{
					assert_and_help(i + 1 < argc);
					std::string mode(argv[i + 1]);

					if(mode == "gm") player.setMode(midisynth::system_mode_gm);
					else if(mode == "gm2") player.setMode(midisynth::system_mode_gm2);
					else if(mode == "gs") player.setMode(midisynth::system_mode_gs);
					else if(mode == "xg") player.setMode(midisynth::system_mode_xg);
					else assert_and_help(true); //hack

					i++;
				}
				break;

			}
		}
		else player.add(argv[i]);
	}

	if(!player.init(playbackRate))
		return 1;

	if(!player.play())
		return 1;

	//Add empty line for better look
	std::cout << std::endl;

	player.close();
	return 0;
}
