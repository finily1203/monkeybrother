#include "GUIAssetBrowser.h"
#include "GlobalCoordinator.h"


std::vector<std::string> AssetBrowser::assetNames;
static float thumbnailSize = 64.0f;
static float paddingSize = 16.0f;
bool AssetBrowser::showTextures = false;
bool AssetBrowser::showShaders = false;
bool AssetBrowser::showAudio = false;
bool AssetBrowser::showFonts = false;

void AssetBrowser::Initialise() {
	//initialise all asset name
	for (auto& asset : assetsManager.getAssetList()) {
		assetNames.push_back(asset);
		std::cout << asset << std::endl;
	}
}

void AssetBrowser::Update() {
	//check if any assets has been added and update the asset list if so
	if (assetsManager.checkIfAssetListChanged()) {
		assetNames.clear();
		for (auto& asset : assetsManager.getAssetList()) {
			assetNames.push_back(asset);
		}
	}

	if (ImGui::Button("Textures")) {
		if(showTextures == false)
			showTextures = true;
		else
			showTextures = false;
	}
	if (showTextures) {
		for (auto& asset : assetNames) {
			if (assetsManager.getTextureList().find(asset) != assetsManager.getTextureList().end()) {
				ImGui::BeginGroup();
				ImGui::Image((void*)assetsManager.GetTexture("fileIcon"), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0});
				ImGui::Text(asset.c_str());
				ImGui::EndGroup();
				ImGui::SameLine();
			}
		}
	}

	ImGui::NewLine();

	if (ImGui::Button("Shaders")) {
		if (showShaders == false)
			showShaders = true;
		else
			showShaders = false;
	}
	if (showShaders) {
		for (auto& asset : assetNames) {
			if (assetsManager.getShaderList().find(asset) != assetsManager.getShaderList().end()) {
				ImGui::BeginGroup();
				ImGui::Image((void*)assetsManager.GetTexture("fileIcon"), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				ImGui::Text(asset.c_str());
				ImGui::EndGroup();
				ImGui::SameLine();
			}
		}
	}

	ImGui::NewLine();

	if (ImGui::Button("Audio")) {
		if (showAudio == false)
			showAudio = true;
		else
			showAudio = false;
	}
	if (showAudio) {
		for (auto& asset : assetNames) {
			if (assetsManager.getAudioList().find(asset) != assetsManager.getAudioList().end()) {
				ImGui::BeginGroup();
				//Experiment 
				if (ImGui::ImageButton(asset.c_str(), (void*)assetsManager.GetTexture("fileIcon"), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0})) {
					audioSystem.playSoundAssetBrowser(asset.c_str());
				}
				ImGui::Text(asset.c_str());
				ImGui::EndGroup();
				ImGui::SameLine();
			}
		}
	}

	ImGui::NewLine();

	if (ImGui::Button("Font")) {
		if (showFonts == false)
			showFonts = true;
		else
			showFonts = false;
	}
	if (showFonts) {
		for (auto& asset : assetNames) {
			if (assetsManager.getFontList().find(asset) != assetsManager.getFontList().end()) {
				ImGui::BeginGroup();
				ImGui::Image((void*)assetsManager.GetTexture("fileIcon"), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				ImGui::Text(asset.c_str());
				ImGui::EndGroup();
				ImGui::SameLine();
			}
		}
	}
}