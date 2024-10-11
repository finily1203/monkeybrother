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

#include "AudioSystem.h"
#include "GlfwFunctions.h"
#include <iostream>

//Default constructor and destructor for AudioSystem class
AudioSystem::AudioSystem() : audioSystem(nullptr), audioSongList(), audioChannel(nullptr), currSongIndex(0) {}
AudioSystem::~AudioSystem() {
    cleanup();
}

//Init function for AudioSystem class to add songs and defaultly play the first song
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

    addSong("../assets/audio/WATER-UNDERWATER_GEN-HDF-25662.wav");
    addSong("../assets/audio/WATER-BUBBLE_GEN-HDF-25446.wav");
    playSong(0);
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
    audioSystem->update();
}

//Clears all the songs from the audioSystem and terminates the audioSystem
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

//Function to add song into the songlist 
void AudioSystem::addSong(const std::string& songPath) {
    FMOD::Sound* audioSong = nullptr;
    FMOD_RESULT result = audioSystem->createSound(songPath.c_str(), FMOD_DEFAULT, nullptr, &audioSong);
    if (result != FMOD_OK) {
        std::cout << "FMOD error! (" << result << ") " << std::endl;
        return;
    }

    audioSongList.push_back(audioSong);
}


//Function to play song at the given index
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

//Getter for currentSongIndex
int AudioSystem::getSongIndex() {
    return currSongIndex;
}

//Setter for currentSongIndex
void AudioSystem::setSongIndex(int index) {
    currSongIndex = index;
}