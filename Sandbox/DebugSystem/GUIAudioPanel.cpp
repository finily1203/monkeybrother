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

	//for (int i = 0; i < channelNum; i++) {
	//	assetsManager.GetAudioSystem()->getChannel(i, &channel);
	//	bool isPlaying = false;
	//	if (channel) {
	//		//check if channel is playing

	//		channel->isPlaying(&isPlaying);
	//		//std::cout << "Channel " << i << " can be found" << std::endl;
	//	}
	//	//std::cout << "is Channel" << i  << " playing ? " << (isPlaying ? "Yes" : "No") << std::endl;
	//}

	//assetsManager.GetAudioSystem()->getChannel(30, &channel);
	//bool isPlaying = false;
	//if (channel) {
	//	channel->isPlaying(&isPlaying);
	//}
	//if (isPlaying) {
	//	channel->getCurrentSound(&sound);
	//	if (sound) {
	//		sound->getName(musicName, sizeof(musicName));
	//		std::cout << "Current music playing: " << musicName << std::endl;
	//	}
	//}

	for (int i = 0; i < 32; i++) {
		assetsManager.GetAudioSystem()->getChannel(i, &channel);
		bool isPlaying = false;
		channel->isPlaying(&isPlaying);
		if (isPlaying) {
			channel->getCurrentSound(&sound);
			if (sound) {
				sound->getName(musicName, sizeof(musicName));
				std::cout << "Current music playing: " << musicName << std::endl;
			}
		}
	}

	ImGui::End();
}

void AudioPanel::Cleanup()
{
}