/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Owen Lew (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIAudioPanel.cpp
@brief  :  This file contains the function definition of the audio panel in the debug system

*Owen Lew (z.lew):
		- creates the audio panel for the debug system
		- users able to see the number of channels and the current music playing in the channel

File Contributions: Owen Lew (100%)

/*_______________________________________________________________________________________________________________*/

#include "GUIAudioPanel.h"
#include "GlobalCoordinator.h"

void AudioPanel::Initialise()
{
}

void AudioPanel::Update()
{
	ImGui::Begin("Audio Panel");
	int channelNum;
	channelNum = static_cast<int>(audioSystem.getChannelList().size());
	
	//display num of channels
	ImGui::Text("Number of Channels: %d", channelNum);

	//display all channels
	for (auto& channel : audioSystem.getChannelList())
	{
		ImGui::Text(channel.first.c_str());
	}

	//display channel current song
	FMOD::Channel* channel = nullptr;
	FMOD::Sound* sound = nullptr;
	char musicName[256] = "No music playing currently"; // default message

	for (int i = 0; i < 32; i++) {
		assetsManager.GetAudioSystem()->getChannel(i, &channel);
		bool isPlaying = false;
		channel->isPlaying(&isPlaying);
		if (isPlaying) {
			channel->getCurrentSound(&sound);
			if (sound) {
				sound->getName(musicName, sizeof(musicName));
				//std::cout << "Current music playing: " << musicName << std::endl;
			}
		}
	}

	ImGui::End();
}

void AudioPanel::Cleanup()
{
}