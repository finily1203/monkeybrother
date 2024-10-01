#include "AudioSystem.h"
#include "GlfwFunctions.h"
#include <iostream>

AudioSystem::AudioSystem() : audioSystem(nullptr), audioSongList(), audioChannel(nullptr), currSongIndex(0) {}
AudioSystem::~AudioSystem() {
    cleanup();
}

void AudioSystem::initialise() {
    FMOD_RESULT result;

    result = FMOD::System_Create(&audioSystem);
    if (result != FMOD_OK) {
        std::cout << "FMOD error! (" << result << ") " << std::endl;
        return;
    }

    result = audioSystem->init(32, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        std::cout << "FMOD error! (" << result << ") " << std::endl;
        return;
    }

    addSong("../assets/audio/Mood_Lofi.wav");
    addSong("../assets/audio/All_My_Fellas.wav");
    playSong(0);
}

void AudioSystem::update() {
    bool bIsPlaying = false;
    if (audioChannel) {
        FMOD_RESULT result = audioChannel->isPlaying(&bIsPlaying);
        if (result != FMOD_OK) {
            std::cout << "FMOD isPlaying error! (" << result << ") " << std::endl;
        }

        // If not playing, replay the sound unless stopAudio is true
        if (!bIsPlaying && !GLFWFunctions::audioStopped && currSongIndex >= 0) {
            playSong(getSongIndex());
        }


        if (GLFWFunctions::audioPaused) {
            // Check if the channel is already paused
            bool isPaused = false;
            audioChannel->getPaused(&isPaused);

            if (!isPaused) {
                // If the sound is not paused, pause it
                FMOD_RESULT result = audioChannel->setPaused(true);
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
                FMOD_RESULT result = audioChannel->setPaused(false);
                if (result != FMOD_OK) {
                    std::cout << "FMOD resume error! (" << result << ")" << std::endl;
                }
                else {
                    std::cout << "Audio resumed." << std::endl;
                }
            }
        }

        if (GLFWFunctions::nextSong) {
            int newIndex = (getSongIndex() + 1) % audioSongList.size();
            playSong(newIndex);

            GLFWFunctions::nextSong = false;
        }

        result = audioChannel->setVolume(GLFWFunctions::volume);
        if (result != FMOD_OK) {
            std::cout << "FMOD setVolume error! (" << result << ")" << std::endl;
        }
    }

    // Step 4: Update the FMOD system
    audioSystem->update();
}

void AudioSystem::cleanup() {
    for (auto song : audioSongList) {
        if (song) {
            song->release();
        }
    }
    audioSongList.clear();

    if (audioSystem) {
        audioSystem->close();
        audioSystem->release();
        audioSystem = nullptr;
    }
}

void AudioSystem::addSong(const std::string& songPath) {
	FMOD::Sound* audioSong = nullptr;
	FMOD_RESULT result = audioSystem->createSound(songPath.c_str(), FMOD_DEFAULT, nullptr, &audioSong);
    if (result != FMOD_OK) {
		std::cout << "FMOD error! (" << result << ") " << std::endl;
		return;
	}

	audioSongList.push_back(audioSong);
}

void AudioSystem::playSong(int index) {
    if (index >= 0 && index < audioSongList.size()) {
        if (audioChannel) {
            FMOD_RESULT result = audioChannel->stop();
            if (result != FMOD_OK) {
				std::cout << "FMOD stop error! (" << result << ") " << std::endl;
			}
            audioChannel = nullptr;
        }
        FMOD_RESULT result = audioSystem->playSound(audioSongList[index], nullptr, false, &audioChannel);
        setSongIndex(index);
        if (result != FMOD_OK) {
			std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
		}
	}
    else {
		std::cout << "Invalid index." << std::endl;
	}
}

int AudioSystem::getSongIndex() {
    return currSongIndex;
}

void AudioSystem::setSongIndex(int index) {
	currSongIndex = index;
}