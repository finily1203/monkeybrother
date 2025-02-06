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

//BGM VOLUME MAX -> 0.1f, INC / DEC BY 0.01f, LOWEST 0f
//SFX VOLUME MAX -> 1.0f, INC / DEC BY 0.1f, LOWEST 0f
//GEN VOLUME MAX -> 0.5f, INC / DEC BY 0.05f, LOWEST 0f

//BGM AFFECTS: bgmChannel, 
//SFX AFFECTS: soundEffectChannel, assetBrowserChannel, pumpChannel, rotationChannel, cutscenePanelChannel
//GEN AFFECTS: ambienceChannel, cutsceneAmbienceChannel, cutsceneHumanChannel

//PUMP CHANNEL MAKE SURE TO MULTIPLY BY 0.1f FOR VOLUME
float AudioSystem::sfxPercentage = 0.f;
float AudioSystem::musicPercentage = 0.f;

//Default constructor and destructor for AudioSystem class
AudioSystem::AudioSystem() : bgmChannel(nullptr), soundEffectChannel(nullptr), assetBrowserChannel(nullptr)
                           , ambienceChannel(nullptr), pumpChannel(nullptr), rotationChannel(nullptr)
                           , cutsceneAmbienceChannel(nullptr), cutsceneAmbienceChannel2(nullptr), cutscenePanelChannel(nullptr), cutsceneHumanChannel(nullptr)
                           , currSongIndex(0), genVol(0.f), bgmVol(0.f), sfxVol(0.f),changeBGM(false), changePanel(false), prevFrame(0)
{

	channelList = new std::vector<std::pair<std::string,FMOD::Channel*>>();

    channelList->push_back(std::make_pair("BGM", bgmChannel));
	channelList->push_back(std::make_pair("SFX", soundEffectChannel));
	channelList->push_back(std::make_pair("AssetBrowser", assetBrowserChannel));
	channelList->push_back(std::make_pair("Ambience", ambienceChannel));
	channelList->push_back(std::make_pair("Pump", pumpChannel));
	channelList->push_back(std::make_pair("Rotation", rotationChannel));

	//std::cout << "Channel List size: " << channelList->size() << std::endl;
}
AudioSystem::~AudioSystem() {}

SystemType AudioSystem::getSystem() {
    return SystemType::AudioSystemType;
}

//Init function for AudioSystem class to add songs and defaultly play the first song
void AudioSystem::initialise() {
    //read from audio JSON file
    readAudioSettingsFromJSON(FilePathManager::GetAudioSettingsJSONPath());
	/*std::cout << sfxPercentage << musicPercentage << std::endl;*/
	setGenVol(musicPercentage);
	setBgmVol(musicPercentage);
	setSfxVol(sfxPercentage);

	std::cout << "Audio System initialised." << std::endl;
    /*genVol(0.35f), bgmVol(0.05f), sfxVol(0.5f)*/
}

//Update function for AudioSystem class to handle pausing, playing of song
//setting volume and to update the song being played
void AudioSystem::update() {
    bool bIsPlaying = false;

	std::cout << cutsceneSystem.getCurrentFrameIndex() << std::endl;

    //if scene is -2 which is cutscene
    if (GameViewWindow::getSceneNum() == -2)
    {
        /*
        * IntroCutscene_Ambience_1: Play at Start Loop
        * IntroCutscene_Panel_2: Play at start of panel
        * IntroCutscene_Human_2: ^ , slowly fade out
        * IntroCutscene_Ambience_2: Play at end of IntroCutscene_Panel_2 audio, loop
        * 3 no audio, just rely on ambience
        * 4 - 8 play at start of panel
        */
		size_t currentFrame = cutsceneSystem.getCurrentFrameIndex();
        if (currentFrame != prevFrame)
        {
            changePanel = false;
        }
        bool isPanelPlaying = false;

        switch (currentFrame) {
        case 0: //scene 1
            if (!cutsceneAmbienceChannel) {
                playCutsceneAmbience("IntroAmbience1");
            }
            break;
        case 1: //scene 2
            if (!cutscenePanelChannel) {
                playCutscenePanel("Panel2");
            }

            if (cutscenePanelChannel) {
                cutscenePanelChannel->isPlaying(&isPanelPlaying);
                if (!isPanelPlaying) {
                    if (!cutsceneAmbienceChannel2) {
                        playCutsceneAmbience2("IntroAmbience2");
                    }
                }
            }

            if (!cutsceneHumanChannel) {
                playCutsceneHuman("HumanAmbience");
            }
            break;
        case 2: //scene 3
            //stop all cutscene audio
            if (cutsceneHumanChannel) {
                FMOD_RESULT result = cutsceneHumanChannel->stop();
                if (result != FMOD_OK) {
                    std::cout << "FMOD stop error for cutscene ambience! (" << result << ")" << std::endl;
                }
                cutsceneHumanChannel = nullptr;
            }
            if (cutscenePanelChannel) {
                FMOD_RESULT result = cutscenePanelChannel->stop();
                if (result != FMOD_OK) {
                    std::cout << "FMOD stop error for cutscene panel! (" << result << ")" << std::endl;
                }
                cutscenePanelChannel = nullptr;
            }
            break;
        case 3: //scene 4
            if (cutsceneSystem.getFrameCompletion(currentFrame)) {
                if (!cutscenePanelChannel) {
                    playCutscenePanel("Panel4_Squeak");
                }
            }
			break;
		case 4: //scene 5
            if (!changePanel) {
                if (cutscenePanelChannel) {
                    FMOD_RESULT result = cutscenePanelChannel->stop();
                    if (result != FMOD_OK) {
                        std::cout << "FMOD stop error for cutscene panel! (" << result << ")" << std::endl;
                    }
                    cutscenePanelChannel = nullptr;
                }
                changePanel = true;
            }
            if (!cutscenePanelChannel) {
                playCutscenePanel("Panel5_Fade");
            }
			break;
		case 5: //scene 6
            if (!changePanel) {
                if (cutscenePanelChannel) {
                    FMOD_RESULT result = cutscenePanelChannel->stop();
                    if (result != FMOD_OK) {
                        std::cout << "FMOD stop error for cutscene panel! (" << result << ")" << std::endl;
                    }
                    cutscenePanelChannel = nullptr;
                }
                changePanel = true;
            }
            if (cutsceneSystem.getFrameCompletion(currentFrame)) {
                if (!cutscenePanelChannel) {
                    playCutscenePanel("Panel6");
                }
            }
			break;
		case 6: //scene 7
            if (!changePanel) {
                if (cutscenePanelChannel) {
                    FMOD_RESULT result = cutscenePanelChannel->stop();
                    if (result != FMOD_OK) {
                        std::cout << "FMOD stop error for cutscene panel! (" << result << ")" << std::endl;
                    }
                    cutscenePanelChannel = nullptr;
                }
                changePanel = true;
            }
            if (cutsceneSystem.getFrameCompletion(currentFrame)) {
                if (!cutscenePanelChannel) {
                    playCutscenePanel("Panel7");
                }
            }
			break;
		case 7: //scene 8
            if (!changePanel) {
                if (cutscenePanelChannel) {
                    FMOD_RESULT result = cutscenePanelChannel->stop();
                    if (result != FMOD_OK) {
                        std::cout << "FMOD stop error for cutscene panel! (" << result << ")" << std::endl;
                    }
                    cutscenePanelChannel = nullptr;
                }
                changePanel = true;
            }
            if (cutsceneSystem.getFrameCompletion(currentFrame)) {
                if (!cutscenePanelChannel) {
                    playCutscenePanel("Panel8");
                }
            }
			break;
        default: // after finishing the cutscene stop all music
            if (cutsceneHumanChannel) {
                FMOD_RESULT result = cutsceneHumanChannel->stop();
                if (result != FMOD_OK) {
                    std::cout << "FMOD stop error for cutscene ambience! (" << result << ")" << std::endl;
                }
                cutsceneHumanChannel = nullptr;
            }
            if (cutscenePanelChannel) {
                FMOD_RESULT result = cutscenePanelChannel->stop();
                if (result != FMOD_OK) {
                    std::cout << "FMOD stop error for cutscene panel! (" << result << ")" << std::endl;
                }
                cutscenePanelChannel = nullptr;
            }
            if (cutsceneAmbienceChannel) {
                FMOD_RESULT result = cutsceneAmbienceChannel->stop();
                if (result != FMOD_OK) {
                    std::cout << "FMOD stop error for cutscene ambience! (" << result << ")" << std::endl;
                }
                cutsceneAmbienceChannel = nullptr;
            }
        }
        prevFrame = currentFrame;
    }

    //main menu audio
    else if (GameViewWindow::getSceneNum() == -1) {
        if (!changeBGM) {
            if (bgmChannel) {
                FMOD_RESULT result = bgmChannel->stop();
                if (result != FMOD_OK) {
                    std::cout << "FMOD stop error for main menu BGM! (" << result << ")" << std::endl;
                }
                bgmChannel = nullptr;
            }
            changeBGM = true;
        }

        bool isBgmPlaying = false;
        if (bgmChannel) {
            bgmChannel->isPlaying(&isBgmPlaying);
        }
        if (!isBgmPlaying) {
            playBgm("mainMenuBGM");
        }
    }

    //only play if scene is 1 or 2
    else if(GameViewWindow::getSceneNum() == 1 || GameViewWindow::getSceneNum() == 2)
    {
        if (!changeBGM) {
            if (bgmChannel) {
                FMOD_RESULT result = bgmChannel->stop();
                if (result != FMOD_OK) {
                    std::cout << "FMOD stop error for main menu BGM! (" << result << ")" << std::endl;
                }
                bgmChannel = nullptr;
            }
			changeBGM = true;
        }

        // Ensure BGM and ambience play only if they are not already playing
        bool isBgmPlaying = false;
        bool isAmbiencePlaying = false;

        if (bgmChannel) {
            bgmChannel->isPlaying(&isBgmPlaying);
        }
        if (ambienceChannel) {
            ambienceChannel->isPlaying(&isAmbiencePlaying);
        }

        if (!isBgmPlaying) {
            playBgm("Iris_L2_BGM_Loop.wav");
        }

        if (!isAmbiencePlaying) {
            playSong("Ambience.wav");
        }

        //std::cout << GameViewWindow::getSceneNum() << std::endl;
        // Check the state of the pump and play/stop the pump sound

        //check if there is a pump entity existing
		for (auto entity : ecsCoordinator.getAllLiveEntities())
		{
			if (ecsCoordinator.hasComponent<PumpComponent>(entity))
			{
				GLFWFunctions::isTherePump = true;
				break;
			}
			else
			{
				GLFWFunctions::isTherePump = false;
			}
		}
        if (GLFWFunctions::isTherePump) {
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

        if ((*GLFWFunctions::keyState)[Key::COMMA]) {
			decAllVol();
            (*GLFWFunctions::keyState)[Key::COMMA] = false;

        }
        else if ((*GLFWFunctions::keyState)[Key::PERIOD]) {
			incAllVol();
            (*GLFWFunctions::keyState)[Key::PERIOD] = false;
        }
    }

    assetsManager.GetAudioSystem()->update();
}

//Clears all the songs from the audioSystem and terminates the audioSystem
void AudioSystem::cleanup() {
    delete channelList;
    channelList = nullptr;

}

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
        ambienceChannel->setVolume(genVol * 5.0f);
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
        pumpChannel->setVolume(sfxVol * 0.1f );
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

void AudioSystem::playCutsceneAmbience(const std::string& ambienceName)
{
    FMOD::Sound* audioSound = assetsManager.GetAudio(ambienceName);
    cutsceneAmbienceChannel = nullptr;
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, false, &cutsceneAmbienceChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }

    if (cutsceneAmbienceChannel) {
        cutsceneAmbienceChannel->setVolume(genVol);
        cutsceneAmbienceChannel->setPaused(false);
    }
}

void AudioSystem::playCutsceneAmbience2(const std::string& ambienceName)
{
	FMOD::Sound* audioSound = assetsManager.GetAudio(ambienceName);
	cutsceneAmbienceChannel2 = nullptr;
	FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, false, &cutsceneAmbienceChannel2);
	if (result != FMOD_OK) {
		std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
	}
	if (cutsceneAmbienceChannel2) {
		cutsceneAmbienceChannel2->setVolume(genVol);
		cutsceneAmbienceChannel2->setPaused(false);
	}
}

void AudioSystem::playCutscenePanel(const std::string& panelName) {
    FMOD::Sound* audioSound = assetsManager.GetAudio(panelName);
    cutscenePanelChannel = nullptr;
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, false, &cutscenePanelChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }

    if (cutscenePanelChannel) {
        cutscenePanelChannel->setVolume(sfxVol);
        cutscenePanelChannel->setPaused(false);
    }
}

void AudioSystem::playCutsceneHuman(const std::string& humanName) {
    FMOD::Sound* audioSound = assetsManager.GetAudio(humanName);
    cutsceneHumanChannel = nullptr;
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, false, &cutsceneHumanChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }
    if (cutsceneHumanChannel) {
        cutsceneHumanChannel->setVolume(genVol * 0.5f);
        cutsceneHumanChannel->setPaused(false);
    }
}

void AudioSystem::decAllVol()
{
    genVol -= 0.05f;
    bgmVol -= 0.01f;
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
    pumpChannel->setVolume(sfxVol * 0.1f);
    rotationChannel->setVolume(sfxVol);
}

void AudioSystem::incAllVol()
{
    genVol += 0.05f;
    bgmVol += 0.01f;
    sfxVol += 0.1f;

    if (genVol > 0.5f)
        genVol = 0.5f;
    if (bgmVol > 0.1f)
        bgmVol = 0.1f;
    if (sfxVol > 0.0f)
        sfxVol = 1.0f;

    ambienceChannel->setVolume(genVol);
    bgmChannel->setVolume(bgmVol);
    soundEffectChannel->setVolume(sfxVol);
    assetBrowserChannel->setVolume(sfxVol);
    pumpChannel->setVolume(sfxVol * 0.1f);
    rotationChannel->setVolume(sfxVol);
}



std::vector<std::pair<std::string, FMOD::Channel*>> AudioSystem::getChannelList() const
{
    return *channelList;
}

float AudioSystem::getGenVol() const {
	return genVol;
}
float AudioSystem::getBgmVol() const {
	return bgmVol;
}
float AudioSystem::getSfxVol() const {
	return sfxVol;
}

void AudioSystem::setGenVol(float volPerc) {
    genVol = (volPerc / 100.0f) * 0.5f;
    ambienceChannel->setVolume(genVol);
	cutsceneAmbienceChannel->setVolume(genVol * 0.1f);
	cutsceneHumanChannel->setVolume(genVol);
}

void AudioSystem::setBgmVol(float volPerc) {
    bgmVol = (volPerc / 100.0f) * 0.1f;
    bgmChannel->setVolume(bgmVol);

}
void AudioSystem::setSfxVol(float volPerc) {
    sfxVol = (volPerc / 100.0f) * 1.0f;
    soundEffectChannel->setVolume(sfxVol);
    assetBrowserChannel->setVolume(sfxVol);
    pumpChannel->setVolume(sfxVol * 0.1f);
    rotationChannel->setVolume(sfxVol);
	cutscenePanelChannel->setVolume(sfxVol);
}

void AudioSystem::readAudioSettingsFromJSON(std::string const& filename)
{
    nlohmann::json audioSettings;
    std::ifstream inputFile(filename);
    if (inputFile.is_open())
    {
        inputFile >> audioSettings;
        inputFile.close();
    }
    sfxPercentage = audioSettings["sfxAudioPercentage"];
    musicPercentage = audioSettings["musicAudioPercentage"];
}

// saving audio settings for sfx and music audio to JSON file
void AudioSystem::saveAudioSettingsToJSON(std::string const& filename, float sfxPercent, float musicPercent)
{
    nlohmann::json audioSettings;

    std::ifstream inputFile(filename);
    if (inputFile.is_open())
    {
        inputFile >> audioSettings;
        inputFile.close();
    }

    audioSettings["sfxAudioPercentage"] = sfxPercent;
    audioSettings["musicAudioPercentage"] = musicPercent;

    std::ofstream outputFile(filename);
    if (!outputFile.is_open())
    {
        return;
    }

    outputFile << audioSettings.dump(2);
    outputFile.close();
}

void AudioSystem::setChangeBGM(bool val)
{
	changeBGM = val;
}