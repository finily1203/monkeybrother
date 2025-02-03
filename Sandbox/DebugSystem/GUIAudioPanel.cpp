#include "GUIAudioPanel.h"
#include "GlobalCoordinator.h"

void AudioPanel::Initialise()
{
}

void AudioPanel::Update()
{
	ImGui::Begin("Audio Panel");
	ImGui::Text("Audio Panel");
	ImGui::End();
}

void AudioPanel::Cleanup()
{
}