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
    channelList->push_back(std::make_pair("SFX_Collection", soundEffectChannel));
    channelList->push_back(std::make_pair("SFX_Bounce", soundEffectChannel));
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

	//std::cout << "Audio System initialised." << std::endl;
    /*genVol(0.35f), bgmVol(0.05f), sfxVol(0.5f)*/
}

//Update function for AudioSystem class to handle pausing, playing of song
//setting volume and to update the song being played
void AudioSystem::update() {
    bool bIsPlaying = false;

	//std::cout << cutsceneSystem.getCurrentFrameIndex() << std::endl;

    //if scene is -2 which is cutscene
    if (GameViewWindow::getSceneNum() == -2)
    {
        if (bgmChannel) {
            FMOD_RESULT result = bgmChannel->stop();
            if (result != FMOD_OK) {
                std::cout << "FMOD stop error for main menu BGM! (" << result << ")" << std::endl;
            }
            bgmChannel = nullptr;
        }
		if (pumpChannel) {
			FMOD_RESULT result = pumpChannel->stop();
			if (result != FMOD_OK) {
				std::cout << "FMOD stop error for pump channel! (" << result << ")" << std::endl;
			}
			pumpChannel = nullptr;
		}

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
        bool isAmbienceOne = false;
		bool isAmbienceTwo = false;

		if (cutsceneAmbienceChannel)
		{
			cutsceneAmbienceChannel->isPlaying(&isAmbienceOne);
			if (!isAmbienceOne)
			{
				playCutsceneAmbience("IntroAmbience1");
			}
		}

		if (cutsceneAmbienceChannel2)
		{
			cutsceneAmbienceChannel2->isPlaying(&isAmbienceTwo);
			if (!isAmbienceTwo)
			{
				playCutsceneAmbience2("IntroAmbience2");
			}
		}

        switch (currentFrame) {
        case 0: //scene 1
            if (!cutsceneAmbienceChannel) {
                std::string ambienceSound = getAudioFileForChannel("CutsceneAmbience", "IntroAmbience1");
                playCutsceneAmbience(ambienceSound);
            }
            break;
        case 1: //scene 2
            if (!cutscenePanelChannel) {
                std::string panelSound = getAudioFileForChannel("CutscenePanel", "Panel2");
                playCutscenePanel(panelSound);
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

    else if (GameViewWindow::getSceneNum() == -4) {
        if (bgmChannel) {
            FMOD_RESULT result = bgmChannel->stop();
            if (result != FMOD_OK) {
                std::cout << "FMOD stop error for main menu BGM! (" << result << ")" << std::endl;
            }
            bgmChannel = nullptr;
        }
        if (pumpChannel) {
            FMOD_RESULT result = pumpChannel->stop();
            if (result != FMOD_OK) {
                std::cout << "FMOD stop error for pump channel! (" << result << ")" << std::endl;
            }
            pumpChannel = nullptr;
        }

        /*
        * EndingCutscene_Ambience_1: Play at Start Loop and play all the way
        * 1 - 7   - play at start of panel
        * 8 and 9 - play at start of panel
        * 10 - 12 - play at start of panel
        * 13 - 14 - play at start of panel
        * 15      - play right after 13 - 14
        */

        size_t currentFrame = cutsceneSystem.getCurrentFrameIndex();
        if (currentFrame != prevFrame)
        {
            changePanel = false;
        }
        bool isPanelPlaying = false;
        bool isAmbience = false;

        if (cutsceneAmbienceChannel)
        {
            cutsceneAmbienceChannel->isPlaying(&isAmbience);
            if (!isAmbience)
            {
                playCutsceneAmbience("EndingCutscene_Ambience_1");
            }
        }

        switch (currentFrame) {
        case 0: //scene 1
            if (!cutsceneAmbienceChannel) {
                playCutsceneAmbience("EndingCutscene_Ambience_1");
            }
            if (!cutscenePanelChannel) {
                playCutscenePanel("EndingCutscene_Panel_1");
            }
            break;
        case 1: //scene 2
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
                playCutscenePanel("EndingCutscene_Panel_2");
            }
            break;
        case 2: //scene 3
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
                playCutscenePanel("EndingCutscene_Panel_3");
            }
            break;
        case 3: //scene 4
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
                playCutscenePanel("EndingCutscene_Panel_4");
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
                playCutscenePanel("EndingCutscene_Panel_5");
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
            if (!cutscenePanelChannel) {
                playCutscenePanel("EndingCutscene_Panel_6");
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
            if (!cutscenePanelChannel) {
                playCutscenePanel("EndingCutscene_Panel_7");
            }
            break;
        case 7: //scene 8 - 9
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
                playCutscenePanel("EndingCutscene_Panel_8");
            }
            break;
        case 8: //scene 10 - 12
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
                playCutscenePanel("EndingCutscene_Panel_10");
            }
            break;
        case 9: //scene 13 - 14
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
                playCutscenePanel("EndingCutscene_Panel_13");
            }
            //check if cutscenePanelChannel is playing
            if (cutscenePanelChannel) {
                cutscenePanelChannel->isPlaying(&isPanelPlaying);
                if (!isPanelPlaying && !GLFWFunctions::endCutsceneLastPanel) {
                    playCutscenePanel("EndingCutscene_Panel_15");
                    GLFWFunctions::endCutsceneLastPanel = true;
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
            std::string bgmSound = getAudioFileForChannel("BGM", "mainMenuBGM");
            playBgm(bgmSound);
        }
    }

    //only play if scene is 1 to 9
    else if (GameViewWindow::getSceneNum() >= 1 && GameViewWindow::getSceneNum() <= 9)
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
            std::string bgmSound = getAudioFileForChannel("BGM", "Iris_L2_BGM_Loop.wav");
            playBgm(bgmSound);
        }

        if (!isAmbiencePlaying) {
            std::string ambienceSound = getAudioFileForChannel("Ambience", "Ambience.wav");
            playSong(ambienceSound);
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
                    std::string pumpSound = getAudioFileForChannel("Pump", "pumpSound.wav");
                    playPumpSound(pumpSound);
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
        }

        if (GLFWFunctions::isRotating) {
            if (!rotationChannel) {
                std::string rotationSound = getAudioFileForChannel("Rotation", "Rotation.wav");
                playRotationEffect(rotationSound);
                rotationChannel->setVolume(0.0f); // Start silent
                currentRotationTime = 0.0f; // Reset fade timer
            }
            else {
                bIsPlaying = false;
                rotationChannel->isPlaying(&bIsPlaying);
                if (!bIsPlaying) {
                    rotationChannel->setPaused(false);
                    currentRotationTime = 0.0f; // Reset fade timer
                }
            }

            // Increment the rotation time counter
            currentRotationTime += GLFWFunctions::delta_time; // You'll need to pass or get deltaTime

            // Calculate fade ratio (0.0 to 1.0)
            float fadeRatio = std::min(currentRotationTime / rotationFadeTime, 1.0f);
            std::cout << fadeRatio << std::endl;

            // Apply the faded volume
            if (rotationChannel) {
                rotationChannel->setVolume(sfxVol * fadeRatio);
            }

            wasRotating = true;
        }
        else {
            if (wasRotating) {
                // Just stopped rotating - could implement fade out here too
                if (rotationChannel) {
                    // Option 1: Stop immediately
                    rotationChannel->stop();
                    rotationChannel = nullptr;

                    // Option 2: Fade out (more complex, would need another timer)
                    // For now we'll just stop immediately
                }
            }
            wasRotating = false;
        }

        if (GLFWFunctions::bumpAudio) {
            std::string bumpSound = getAudioFileForChannel("SFX_Bounce", "Mossball_Bounce.wav");
            playSoundEffect(bumpSound);
            GLFWFunctions::bumpAudio = false;
            std::cout << "Bump audio played." << std::endl;
        }

        if (GLFWFunctions::collectAudio) {
            std::string collectSound = getAudioFileForChannel("SFX_Collection", "Collection.wav");
            playSoundEffect(collectSound);
            GLFWFunctions::collectAudio = false;
        }

        if (GLFWFunctions::attackAudio) {
            playSoundEffect("Fish_Attack.wav");
            GLFWFunctions::attackAudio = false;
        }

        if (GLFWFunctions::filterExitAudio) {
            playSoundEffect("Filter_Exit.wav");
            GLFWFunctions::filterExitAudio = false;
        }
    }

    assetsManager.GetAudioSystem()->update();
}

//Clears all the songs from the audioSystem and terminates the audioSystem
void AudioSystem::cleanup() {
    delete channelList;
    channelList = nullptr;

}

//Function to play song of given name 
void AudioSystem::playSong(const std::string& songName) {
    std::string customChannel = getCustomChannelForAudio(songName);
    if (!customChannel.empty() && customChannel != "Ambience") {
        // Use the custom mapping instead
        playAudioByMapping(songName, customChannel);
        return;
    }
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

// function to play BGM of given name
void AudioSystem::playBgm(const std::string& songName) {

    // Check if there's a custom channel mapping for this audio
    std::string customChannel = getCustomChannelForAudio(songName);
    if (!customChannel.empty() && customChannel != "BGM") {
        // Use the custom mapping instead
        playAudioByMapping(songName, customChannel);
        return;
    }

    FMOD::Sound* audioSong = assetsManager.GetAudio(songName);
	//std::cout << songName << std::endl;

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

// Function to play the pump sound of given name
void AudioSystem::playPumpSound(const std::string& soundName)
{

    // Check if there's a custom channel mapping for this audio
    std::string customChannel = getCustomChannelForAudio(soundName);
    if (!customChannel.empty() && customChannel != "Pump") {
        // Use the custom mapping instead
        playAudioByMapping(soundName, customChannel);
        return;
    }


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

// Function to play sound effect of given name
void AudioSystem::playSoundEffect(const std::string& soundEffectName)
{

    // Check if there's a custom channel mapping for this audio
    std::string customChannel = getCustomChannelForAudio(soundEffectName);
    if (!customChannel.empty() && customChannel != "SFX_Collection" && customChannel != "SFX_Bounce") {
        // Use the custom mapping instead
        playAudioByMapping(soundEffectName, customChannel);
        return;
    }

    FMOD::Sound* audioSound = assetsManager.GetAudio(soundEffectName);
    soundEffectChannel = nullptr;
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, false, &soundEffectChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }

    if (soundEffectChannel) {
        if (soundEffectName == "Fish_Attack.wav")
        {
            soundEffectChannel->setVolume(sfxVol * 2.f);
        }
        else
        {
            soundEffectChannel->setVolume(sfxVol);
        }
        soundEffectChannel->setPaused(false);
    }
}

// function to play rotation effect of given name
void AudioSystem::playRotationEffect(const std::string& soundEffectName)
{
    // Check if there's a custom channel mapping for this audio
    std::string customChannel = getCustomChannelForAudio(soundEffectName);
    if (!customChannel.empty() && customChannel != "Rotation") {
        // Use the custom mapping instead
        playAudioByMapping(soundEffectName, customChannel);
        return;
    }

    FMOD::Sound* audioSound = assetsManager.GetAudio(soundEffectName);
    rotationChannel = nullptr;
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, true, &rotationChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }
    if (rotationChannel) {
        rotationChannel->setVolume(0.0f); // Start at zero volume to avoid popping
        rotationChannel->setPaused(false);
    }
}

// function to play sound asset browser of given name
void AudioSystem::playSoundAssetBrowser(const std::string& soundName)
{
    // Check if there's a custom channel mapping for this audio
    std::string customChannel = getCustomChannelForAudio(soundName);
    if (!customChannel.empty() && customChannel != "AssetBrowser") {
        // Use the custom mapping instead
        playAudioByMapping(soundName, customChannel);
        return;
    }

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

// function to play cutscene ambience of given name
void AudioSystem::playCutsceneAmbience(const std::string& ambienceName)
{
    // Check if there's a custom channel mapping for this audio
    std::string customChannel = getCustomChannelForAudio(ambienceName);
    if (!customChannel.empty()) {
        // Use the custom mapping instead
        playAudioByMapping(ambienceName, customChannel);
        return;
    }

    FMOD::Sound* audioSound = assetsManager.GetAudio(ambienceName);
    cutsceneAmbienceChannel = nullptr;
    FMOD_RESULT result = assetsManager.GetAudioSystem()->playSound(audioSound, nullptr, false, &cutsceneAmbienceChannel);
    if (result != FMOD_OK) {
        std::cout << "FMOD playSound error! (" << result << ") " << std::endl;
    }

    if (cutsceneAmbienceChannel) {
        if (ambienceName == "EndingCutscene_Ambience_1")
            cutsceneAmbienceChannel->setVolume(genVol * 3.0f);
        else
            cutsceneAmbienceChannel->setVolume(genVol * 2.0f);
        cutsceneAmbienceChannel->setPaused(false);
    }
}

// function to play cutscene ambience 2 of given name
void AudioSystem::playCutsceneAmbience2(const std::string& ambienceName)
{
    // Check if there's a custom channel mapping for this audio
    std::string customChannel = getCustomChannelForAudio(ambienceName);
    if (!customChannel.empty()) {
        // Use the custom mapping instead
        playAudioByMapping(ambienceName, customChannel);
        return;
    }

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

// function to play cutscene panel of given name
void AudioSystem::playCutscenePanel(const std::string& panelName) {
    // Check if there's a custom channel mapping for this audio
    std::string customChannel = getCustomChannelForAudio(panelName);
    if (!customChannel.empty()) {
        // Use the custom mapping instead
        playAudioByMapping(panelName, customChannel);
        return;
    }

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

// function to play cutscene human of given name
void AudioSystem::playCutsceneHuman(const std::string& humanName) {

    // Check if there's a custom channel mapping for this audio
    std::string customChannel = getCustomChannelForAudio(humanName);
    if (!customChannel.empty()) {
        // Use the custom mapping instead
        playAudioByMapping(humanName, customChannel);
        return;
    }

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

// function to decrease volume for all audio channels
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

// function to increase volume for all audio channels
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


// retrieve the vector of audio channels
std::vector<std::pair<std::string, FMOD::Channel*>> AudioSystem::getChannelList() const
{
    return *channelList;
}

// retrieve the general audio volume
float AudioSystem::getGenVol() const {
	return genVol;
}
// retrieve the BGM audio volume
float AudioSystem::getBgmVol() const {
	return bgmVol;
}
// retrieve the SFX audio volume
float AudioSystem::getSfxVol() const {
	return sfxVol;
}
// set the general audio volume
void AudioSystem::setGenVol(float volPerc) {
    genVol = (volPerc / 100.0f) * 0.5f;
    ambienceChannel->setVolume(genVol);
	cutsceneAmbienceChannel->setVolume(genVol * 0.1f);
	cutsceneHumanChannel->setVolume(genVol);
}

// set the BGM audio volume
void AudioSystem::setBgmVol(float volPerc) {
    bgmVol = (volPerc / 100.0f) * 0.1f;
    bgmChannel->setVolume(bgmVol);

}

// set the SFX audio volume
void AudioSystem::setSfxVol(float volPerc) {
    sfxVol = (volPerc / 100.0f) * 1.0f;
    soundEffectChannel->setVolume(sfxVol);
    assetBrowserChannel->setVolume(sfxVol);
    pumpChannel->setVolume(sfxVol * 0.1f);
    rotationChannel->setVolume(sfxVol);
	cutscenePanelChannel->setVolume(sfxVol);
}

// read audio settings for sfx and music audio from JSON file
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

// set the changeBGM boolean
void AudioSystem::setChangeBGM(bool val)
{
	changeBGM = val;
}

void AudioSystem::playAudioByMapping(const std::string& audioName, const std::string& channelName) {
    if (channelName == "BGM") {
        playBgm(audioName);
    }
    else if (channelName == "SFX") {
        playSoundEffect(audioName);
    }
    else if (channelName == "AssetBrowser") {
        playSoundAssetBrowser(audioName);
    }
    else if (channelName == "Ambience") {
        playSong(audioName);
    }
    else if (channelName == "Pump") {
        playPumpSound(audioName);
    }
    else if (channelName == "Rotation") {
        playRotationEffect(audioName);
    }
}

bool AudioSystem::isAudioPlayingOnChannel(const std::string& channelName) {
    bool isPlaying = false;

    FMOD::Channel* channel = nullptr;

    if (channelName == "BGM") {
        channel = bgmChannel;
    }
    else if (channelName == "SFX") {
        channel = soundEffectChannel;
    }
    else if (channelName == "AssetBrowser") {
        channel = assetBrowserChannel;
    }
    else if (channelName == "Ambience") {
        channel = ambienceChannel;
    }
    else if (channelName == "Pump") {
        channel = pumpChannel;
    }
    else if (channelName == "Rotation") {
        channel = rotationChannel;
    }

    if (channel) {
        channel->isPlaying(&isPlaying);
    }

    return isPlaying;
}

std::string AudioSystem::getCustomChannelForAudio(const std::string& audioName) {
    // Get the audio mappings from GameViewWindow
    auto& mappings = *GameViewWindow::audioChannelMappings;

    // Check if this audio has a custom mapping
    if (mappings.find(audioName) != mappings.end()) {
        return mappings[audioName];
    }

    // No custom mapping found
    return "";
}

std::string AudioSystem::getAudioFileForChannel(const std::string& channelName, const std::string& defaultFile) {
    // Look through mappings to find an audio file that maps to this channel
    if(defaultFile == "Iris_L2_BGM_Loop.wav") return defaultFile;
	if (defaultFile == "mainMenuBGM") return defaultFile;

    for (const auto& mapping : *GameViewWindow::audioChannelMappings) {
        if (mapping.second == channelName) {
            return mapping.first;
        }
    }
    // Return the default if no mapping found
    return defaultFile;
}