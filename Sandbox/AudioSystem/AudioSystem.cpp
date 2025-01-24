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
#include "GUIGameViewport.h"

//Default constructor and destructor for AudioSystem class
AudioSystem::AudioSystem() : bgmChannel(nullptr), soundEffectChannel(nullptr), assetBrowserChannel(nullptr)
                           , ambienceChannel(nullptr), pumpChannel(nullptr), rotationChannel(nullptr)
                           , currSongIndex(0), genVol(0.35f), bgmVol(0.05f), sfxVol(1.0f) {}
AudioSystem::~AudioSystem() {
    cleanup();
}

SystemType AudioSystem::getSystem() {
    return SystemType::AudioSystemType;
}

//Init function for AudioSystem class to add songs and defaultly play the first song
void AudioSystem::initialise() {

    playSong("Ambience.wav");
    playBgm("Iris_L2_BGM_Loop.wav");
}

//Update function for AudioSystem class to handle pausing, playing of song
//setting volume and to update the song being played
void AudioSystem::update() {
    bool bIsPlaying = false;

    // Check the state of the pump and play/stop the pump sound
    if (GLFWFunctions::isPumpOn) {
        if (!pumpChannel) {
            playPumpSound("pumpSound.wav");
        }
        else {
            // Ensure the pump sound is playing
            FMOD_RESULT result = pumpChannel->isPlaying(&bIsPlaying);
            if (result != FMOD_OK) {
                std::cout << "FMOD isPlaying error for pump sound! (" << result << ")" << std::endl;
            }

            if (!bIsPlaying) {
                pumpChannel->setPaused(false); // Resume if paused
            }
        }
    }
    else {
        // Stop the pump sound if it's no longer needed
        if (pumpChannel) {
            FMOD_RESULT result = pumpChannel->stop();
            if (result != FMOD_OK) {
                std::cout << "FMOD stop error for pump sound! (" << result << ")" << std::endl;
            }
            pumpChannel = nullptr;
        }
    }


    if (bgmChannel) {
        FMOD_RESULT result = bgmChannel->isPlaying(&bIsPlaying);
        if (result != FMOD_OK) {
            std::cout << "FMOD isPlaying error! (" << result << ") " << std::endl;
        }

        // If not playing, replay the sound unless stopAudio is true
        if (!bIsPlaying && !GLFWFunctions::audioStopped && currSongIndex >= 0) {
            playSong("Ambience.wav");
        }

        if (GLFWFunctions::audioPaused || GameViewWindow::getPaused()) {
            // Check if the channel is already paused
            bool isPaused = false;
            bgmChannel->getPaused(&isPaused);

            if (!isPaused) {
                // If the sound is not paused, pause it
                result = bgmChannel->setPaused(true);
                result = soundEffectChannel->setPaused(true);
                result = assetBrowserChannel->setPaused(true);
                result = ambienceChannel->setPaused(true);
                result = pumpChannel->setPaused(true);
                result = rotationChannel->setPaused(true);
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
            bgmChannel->getPaused(&isPaused);

            if (isPaused) {
                // If the sound is paused, unpause it
                result = bgmChannel->setPaused(false);
                result = soundEffectChannel->setPaused(false);
                result = assetBrowserChannel->setPaused(false);
                result = ambienceChannel->setPaused(false);
                result = pumpChannel->setPaused(false);
                result = rotationChannel->setPaused(false);
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
    }

    if (GLFWFunctions::isRotating) {
        if (!rotationChannel) {
            playRotationEffect("Rotation.wav");
        }
        else {
            bIsPlaying = false;
            rotationChannel->isPlaying(&bIsPlaying);
            if (!bIsPlaying) {
                rotationChannel->setPaused(false);
            }
        }
    }
    else {
        if (rotationChannel) {
            rotationChannel->setPaused(true);
        }
        rotationChannel = nullptr;
    }

    if (GLFWFunctions::bumpAudio) {
        playSoundEffect("Mossball_Bounce.wav");
        GLFWFunctions::bumpAudio = false;

		std::cout << "Bump audio played." << std::endl;
    }

    if (GLFWFunctions::collectAudio) {
        playSoundEffect("Collection.wav");
        GLFWFunctions::collectAudio = false;
    }

    if ((*GLFWFunctions::keyState)[Key::NUM_9] && (GLFWFunctions::debug_flag == false)) {
        playSoundEffect("bubbleButton");
        (*GLFWFunctions::keyState)[Key::NUM_9] = false;
    }

     else if ((*GLFWFunctions::keyState)[Key::NUM_8] && (GLFWFunctions::debug_flag == false)) {
            playSoundEffect("bubbleSingle");
            (*GLFWFunctions::keyState)[Key::NUM_8] = false;
    }

    if ((*GLFWFunctions::keyState)[Key::COMMA]) {
        genVol -= 0.1f;
        bgmVol -= 0.05f;
		sfxVol -= 0.1f;

        if (genVol < 0.0f) 
            genVol = 0.0f;
        if (bgmVol < 0.0f) 
            bgmVol = 0.0f;
		if (sfxVol < 0.0f)
			sfxVol = 0.0f;

        ambienceChannel->setVolume(genVol);
        bgmChannel->setVolume(bgmVol);
		soundEffectChannel->setVolume(sfxVol);
		assetBrowserChannel->setVolume(sfxVol);
        pumpChannel->setVolume(sfxVol);
        rotationChannel->setVolume(sfxVol);
        
        (*GLFWFunctions::keyState)[Key::COMMA] = false;

    }
    else if ((*GLFWFunctions::keyState)[Key::PERIOD]) {
        genVol += 0.1f;
        bgmVol += 0.05f;
        sfxVol += 0.1f;

        if (genVol > 1.0f) 
            genVol = 1.0f;
        if (bgmVol > 0.5f) 
            bgmVol = 0.5f;
        if (sfxVol > 0.0f)
            sfxVol = 1.0f;

        ambienceChannel->setVolume(genVol);
        bgmChannel->setVolume(bgmVol);
        soundEffectChannel->setVolume(sfxVol);
        assetBrowserChannel->setVolume(sfxVol);
        pumpChannel->setVolume(sfxVol);
        rotationChannel->setVolume(sfxVol);
 
        (*GLFWFunctions::keyState)[Key::PERIOD] = false;
    }

    assetsManager.GetAudioSystem()->update();
}

//Clears all the songs from the audioSystem and terminates the audioSystem
void AudioSystem::cleanup() {}

void AudioSystem::playSong(const std::string& songName) {
    FMOD::Sound* audioSong = assetsManager.GetAudio(songName);
    if (ambienceChannel) {
        FMOD_RESULT result = ambienceChannel->stop();
        if (result != FMOD_OK) {
            std::cout << "FMOD stop error! (" << result << ") " << std::endl;
        }
        ambienceChannel = nullptr;
    }
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSong, nullptr, true, &ambienceChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }

    if (ambienceChannel) {
        ambienceChannel->setVolume(genVol);
        ambienceChannel->setPaused(false);
    }

}

void AudioSystem::playBgm(const std::string& songName) {
    FMOD::Sound* audioSong = assetsManager.GetAudio(songName);


    if (bgmChannel) {
        FMOD_RESULT result = bgmChannel->stop();
        if (result != FMOD_OK) {
            std::cout << "FMOD stop error! (" << result << ") " << std::endl;
        }
        bgmChannel = nullptr;
    }

    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSong, nullptr, true, &bgmChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }

    if (bgmChannel) {
		bgmChannel->setVolume(bgmVol);
		bgmChannel->setPaused(false);
    }

}

void AudioSystem::playPumpSound(const std::string& soundName)
{
    FMOD::Sound* audioSong = assetsManager.GetAudio(soundName);

    if (pumpChannel) {
        FMOD_RESULT result = pumpChannel->stop();
        if (result != FMOD_OK) {
            std::cout << "FMOD stop error! (" << result << ") " << std::endl;
        }
        pumpChannel = nullptr;
    }

    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSong, nullptr, true, &pumpChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }

    if (pumpChannel) {
        pumpChannel->setVolume(genVol);
        pumpChannel->setPaused(false);
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

    if (soundEffectChannel) {
        soundEffectChannel->setVolume(sfxVol);
        soundEffectChannel->setPaused(false);
    }
}

void AudioSystem::playRotationEffect(const std::string& soundEffectName)
{
    FMOD::Sound* audioSound = assetsManager.GetAudio(soundEffectName);
    rotationChannel = nullptr;
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, false, &rotationChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }

    if (rotationChannel) {
        rotationChannel->setVolume(sfxVol);
        rotationChannel->setPaused(false);
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

    if (assetBrowserChannel) {
        assetBrowserChannel->setVolume(sfxVol);
        assetBrowserChannel->setPaused(false);
    }
}

