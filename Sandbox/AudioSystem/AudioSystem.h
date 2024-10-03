#pragma once
#include "Systems.h"
#include "fmod.hpp"
#include <vector>
#include <string>

class AudioSystem : public GameSystems
{
public:
	AudioSystem();
	~AudioSystem();

	//initialise, update and cleanup function for window class 
	//override the virtual function for systems
	void initialise() override;
	void update() override;
	void cleanup() override;

	void addSong(const std::string& songPath);
	void playSong(int index);
	int getSongIndex();
	void setSongIndex(int index);

private:
	FMOD::System* audioSystem;
	FMOD::Channel* audioChannel;
	std::vector<FMOD::Sound*> audioSongList;
	int currSongIndex;
};