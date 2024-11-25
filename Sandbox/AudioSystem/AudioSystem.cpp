/*!
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author: Joel Chu (c.weiyuan)
@team:   MonkeHood
@course: CSD2401
@file:   AudioSystem.cpp
@brief:  This source file includes all the implementation of the AudioSystem class.
         AudioSystem class is a child class of GameSystems, which is responsible for
         the audio system of the game. It uses FMOD library to handle audio files.
         Joel Chu (c.weiyuan): Implemented all of the functions that belongs to
                               the AudioSystem class.
                               100%
*//*___________________________________________________________________________-*/

#include "GlobalCoordinator.h"
#include "AudioSystem.h"
#include "GlfwFunctions.h"
#include <iostream>

//Default constructor and destructor for AudioSystem class
AudioSystem::AudioSystem() : audioChannel(nullptr), soundEffectChannel(nullptr), assetBrowserChannel(nullptr), currSongIndex(0), volume(0.1f) {}
AudioSystem::~AudioSystem() {
    cleanup();
}

SystemType AudioSystem::getSystem() {
    return SystemType::AudioSystemType;
}

//Init function for AudioSystem class to add songs and defaultly play the first song
void AudioSystem::initialise() {
    //FMOD_RESULT result;

    //result = FMOD::System_Create(&audioSystem);
    //if (result != FMOD_OK) {
    //    std::cout << "FMOD error! (" << result << ") " << std::endl;
    //    return;
    //}

    //result = audioSystem->init(32, FMOD_INIT_NORMAL, nullptr);
    //if (result != FMOD_OK) {
    //    std::cout << "FMOD error! (" << result << ") " << std::endl;
    //    return;
    //}

    //loadAudioAssets();

    playSong("Ambience.wav");
}

//Update function for AudioSystem class to handle pausing, playing of song
//setting volume and to update the song being played
void AudioSystem::update() {
    bool bIsPlaying = false;
    if (audioChannel) {
        FMOD_RESULT result = audioChannel->isPlaying(&bIsPlaying);
        if (result != FMOD_OK) {
            std::cout << "FMOD isPlaying error! (" << result << ") " << std::endl;
        }

        // If not playing, replay the sound unless stopAudio is true
        if (!bIsPlaying && !GLFWFunctions::audioStopped && currSongIndex >= 0) {
            playSong("Ambience.wav");
        }


        if (GLFWFunctions::audioPaused) {
            // Check if the channel is already paused
            bool isPaused = false;
            audioChannel->getPaused(&isPaused);

            if (!isPaused) {
                // If the sound is not paused, pause it
                result = audioChannel->setPaused(true);
                if (result != FMOD_OK) {
                    std::cout << "FMOD pause error! (" << result << ")" << std::endl;
                }
                else {
                    std::cout << "Audio paused." << std::endl;
                }
            }
        }
        else {
            // Check if the channel is already playing (not paused)
            bool isPaused = false;
            audioChannel->getPaused(&isPaused);

            if (isPaused) {
                // If the sound is paused, unpause it
                result = audioChannel->setPaused(false);
                if (result != FMOD_OK) {
                    std::cout << "FMOD resume error! (" << result << ")" << std::endl;
                }
                else {
                    std::cout << "Audio resumed." << std::endl;
                }
            }
        }

        if (GLFWFunctions::audioNext) {
            switch (GLFWFunctions::audioNum) {
            case 0:
                playSong("Ambience.wav");
                break;
            case 1:
                playSong("bubbles");
                break;
            }
            GLFWFunctions::audioNext = false;
        }

        if (GLFWFunctions::keyState[Key::COMMA]) {
            volume -= 0.1f;
            if (volume < 0.0f) {
                volume = 0.0f;
            }
            GLFWFunctions::keyState[Key::COMMA] = false;
            std::cout << volume << std::endl;
        }
        else if (GLFWFunctions::keyState[Key::PERIOD]) {
            volume += 0.1f;
            if (volume > 1.0f) {
                volume = 1.0f;
            }
            GLFWFunctions::keyState[Key::PERIOD] = false;
            std::cout << volume << std::endl;
        }

        result = audioChannel->setVolume(volume);
        if (result != FMOD_OK) {
            std::cout << "FMOD setVolume error! (" << result << ")" << std::endl;
        }
    }


    if (GLFWFunctions::bumpAudio) {
        playSoundEffect("Mossball_Bounce.wav");
        GLFWFunctions::bumpAudio = false;
    }

    if (GLFWFunctions::collectAudio) {
        playSoundEffect("bubbleButton");
        GLFWFunctions::collectAudio = false;
    }

    if (GLFWFunctions::keyState[Key::NUM_9] && (GLFWFunctions::debug_flag == false)) {
        playSoundEffect("bubbleButton");
        GLFWFunctions::keyState[Key::NUM_9] = false;
    }

    if (GLFWFunctions::keyState[Key::NUM_0] && (GLFWFunctions::debug_flag == false)) {
        playSoundEffect("bubbleSingle");
        GLFWFunctions::keyState[Key::NUM_0] = false;
    }

    assetsManager.GetAudioSystem()->update();
}

//Clears all the songs from the audioSystem and terminates the audioSystem
void AudioSystem::cleanup() {
    //for (auto song : audioSongList) {
    //    if (song) {
    //        song->release();
    //    }
    //}
    //audioSongList.clear();

    //if (audioSystem) {
    //    audioSystem->close();
    //    audioSystem->release();
    //    audioSystem = nullptr;
    //}
}

void AudioSystem::playSong(const std::string& songName) {
    FMOD::Sound* audioSong = assetsManager.GetAudio(songName);
    if (audioChannel) {
        FMOD_RESULT result = audioChannel->stop();
        if (result != FMOD_OK) {
            std::cout << "FMOD stop error! (" << result << ") " << std::endl;
        }
        audioChannel = nullptr;
    }
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSong, nullptr, false, &audioChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }

}


void AudioSystem::playSoundEffect(const std::string& soundEffectName)
{
    FMOD::Sound* audioSound = assetsManager.GetAudio(soundEffectName);
    soundEffectChannel = nullptr;
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, false, &soundEffectChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }
}

void AudioSystem::playSoundAssetBrowser(const std::string& soundName)
{
	FMOD::Sound* audioSound = assetsManager.GetAudio(soundName);
	assetBrowserChannel = nullptr;
	FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, false, &assetBrowserChannel);
    if (result != FMOD_OK) {
		std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
	}
}


void AudioSystem::loadAudioAssets() const
{
    //std::string jsonFilePath = FilePathManager::GetAssetsJSONPath();
    //std::ifstream file(jsonFilePath);
    //nlohmann::json jsonObj;

    //if (file.is_open())
    //{
    //    file >> jsonObj;
    //    file.close();
    //}

    //for (const auto& audioAsset : jsonObj["audioAssets"])
    //{
    //    std::string audioName = audioAsset["audioName"].get<std::string>();
    //    std::string relativePath = audioAsset["filePath"].get<std::string>();

    //    std::string audioFilePath = FilePathManager::GetExecutablePath() + "\\..\\..\\..\\" + relativePath;
    //    assetsManager.LoadAudio(audioName, audioFilePath, audioSystem);
    //}
}