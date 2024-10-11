/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   AudioSystem.h
@brief:  This header file includes all the declaration of the AudioSystem class.
		 Joel Chu (c.weiyuan): Declared all the functions in AudioSystem and 
							   integrated FMOD lib to handle audio files.
							   100%
*//*___________________________________________________________________________-*/
#pragma once
#include "Systems.h"
#include "fmod.hpp"
#include <vector>
#include <string>

class AudioSystem : public GameSystems
{
public:
	//Default constructor and destructor for AudioSystem class
	AudioSystem();
	~AudioSystem();

	//Init function for AudioSystem class to add songs and defaultly play the first song
	void initialise() override;

	//Update function for AudioSystem class to handle pausing, playing of song
	//setting volume and to update the song being played
	void update() override;
	
	//Clears all the songs from the audioSystem and terminates the audioSystem
	void cleanup() override;

	//Function to add song into the songlist 
	void addSong(const std::string& songPath);

	//Function to play song at the given index
	void playSong(int index);

	//Getter for currentSongIndex
	int getSongIndex();
	//Setter for currentSongIndex
	void setSongIndex(int index);

private:
	FMOD::System* audioSystem;
	FMOD::Channel* audioChannel;
	std::vector<FMOD::Sound*> audioSongList;
	int currSongIndex;
};