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
#include "EngineDefinitions.h"
#include "Systems.h"
#include "fmod.hpp"
#include <vector>
#include <string>
#include "GlobalCoordinator.h"

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
	SystemType getSystem() override; //For perfomance viewer

	std::vector<std::pair<std::string, FMOD::Channel*>> getChannelList() const;

	//Function to add song into the songlist 
	//void addSong(const std::string& songPath);

	//Function to play song at the given index
	//void playSong(int index);
	void playSong(const std::string& songName);

	void playBgm(const std::string& songName);

	void playPumpSound(const std::string& songName);

	//Function to play sound effect
	void playSoundEffect(const std::string& soundEffectName);

	//Function to play rotation sound effect
	void playRotationEffect(const std::string& soundEffectName);

	//Function to play sound from asset browser
	void playSoundAssetBrowser(const std::string& soundName);

	FMOD::Channel* getPumpChannel() const { return pumpChannel; }	
	void setPumpChannelToNull() { pumpChannel = nullptr; }

	void decAllVol();
	void incAllVol();

	float getGenVol() const;
	float getBgmVol() const;
	float getSfxVol() const;

	void setGenVol(float volPerc);
	void setBgmVol(float volPerc);
	void setSfxVol(float volPerc);

	void saveAudioSettingsToJSON(std::string const& filename, float sfxPercentage, float musicPercentage);

private:
	//FMOD::System* audioSystem;
	/*std::unordered_map<std::string, FMOD::Channel*> channelList;*/
	/*std::vector<std::pair<std::string, FMOD::Channel*>> channelList;*/

	std::vector<std::pair<std::string,FMOD::Channel*>>* channelList;

	FMOD::Channel* bgmChannel;
	FMOD::Channel* soundEffectChannel;
	FMOD::Channel* assetBrowserChannel;
	FMOD::Channel* ambienceChannel;
	FMOD::Channel* pumpChannel;
	FMOD::Channel* rotationChannel;

	/*
	* IntroCutscene_Ambience_1: Play at Start Loop
	* IntroCutscene_Panel_2: Play at start of panel
	* IntroCutscene_Human_2: ^ , slowly fade out
	* IntroCutscene_Ambience_2: Play at end of IntroCutscene_Panel_2 audio, loop
	* 3 no audio, just rely on ambience
	* 4 - 8 play at start of panel
	*/

	FMOD::ChannelGroup* cutsceneGroup;

	FMOD::Channel* cutsceneAmbienceChannel; // loops, switch when panel 2 starts
	FMOD::Channel* cutscenePanelChannel; //play one time audio per panel
	FMOD::Channel* cutsceneHumanChannel; //play once at panel 2, but should fade out (can try)

	//std::vector<FMOD::Sound*> audioSongList;
	int currSongIndex;
	float genVol; // general volume
	float bgmVol; // bgm volume
	float sfxVol; // sound effect volume
};