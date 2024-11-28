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

	float availWidth = ImGui::GetContentRegionAvail().x - 200.f;
	int columns = static_cast<int>(availWidth / (thumbnailSize + paddingSize));
	int currItemIndex = 0;

	if (ImGui::Button("Textures")) {
		if(showTextures == false)
			showTextures = true;
		else
			showTextures = false;
	}
	if (showTextures) {
		if (columns < 1) columns = 1;
		for (auto& asset : assetNames) {
			if (assetsManager.getTextureList().find(asset) != assetsManager.getTextureList().end()) {
				ImGui::BeginGroup();
				ImGui::Image((void*)assetsManager.GetTexture("fileIcon"), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					ImGui::SetDragDropPayload("TEXTURE_PAYLOAD", asset.c_str(), asset.size() + 1, ImGuiCond_Once);
					ImGui::Text("Dragging: %s", asset.c_str());
					ImGui::EndDragDropSource();
				}
				/*ImGui::Text(asset.c_str());*/
                std::string trunStr = cutString(asset, thumbnailSize);
				ImGui::Text(trunStr.c_str());

				ImGui::EndGroup();

				currItemIndex++;
				if (currItemIndex % columns != 0) {
					ImGui::SameLine(); // Place the next item in the same row
				}
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
		if (columns < 1) columns = 1;
		for (auto& asset : assetNames) {
			if (assetsManager.getShaderList().find(asset) != assetsManager.getShaderList().end()) {
				ImGui::BeginGroup();
				ImGui::Image((void*)assetsManager.GetTexture("fileIcon"), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					ImGui::SetDragDropPayload("SHADER_PAYLOAD", asset.c_str(), asset.size() + 1, ImGuiCond_Once);
					ImGui::Text("Dragging: %s", asset.c_str());
					ImGui::EndDragDropSource();
				}
				/*ImGui::Text(asset.c_str());*/
				std::string trunStr = cutString(asset, thumbnailSize);
				ImGui::Text(trunStr.c_str());

				ImGui::EndGroup();

				currItemIndex++;
				if (currItemIndex % columns != 0) {
					ImGui::SameLine(); // Place the next item in the same row
				}
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
		if (columns < 1) columns = 1;
		for (auto& asset : assetNames) {
			if (assetsManager.getAudioList().find(asset) != assetsManager.getAudioList().end()) {
				ImGui::BeginGroup();
				if (ImGui::ImageButton(asset.c_str(), (void*)assetsManager.GetTexture("fileIcon"), {thumbnailSize, thumbnailSize}, {0, 1}, {1, 0})) {
					audioSystem.playSoundAssetBrowser(asset.c_str());
				}
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					ImGui::SetDragDropPayload("AUDIO_PAYLOAD", asset.c_str(), asset.size() + 1, ImGuiCond_Once);
					ImGui::Text("Dragging: %s", asset.c_str());
					ImGui::EndDragDropSource();
				}
				/*ImGui::Text(asset.c_str());*/
				std::string trunStr = cutString(asset, thumbnailSize);
				ImGui::Text(trunStr.c_str());

				ImGui::EndGroup();

				currItemIndex++;
				if (currItemIndex % columns != 0) {
					ImGui::SameLine(); // Place the next item in the same row
				}
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
		if (columns < 1) columns = 1;
		for (auto& asset : assetNames) {
			if (assetsManager.getFontList().find(asset) != assetsManager.getFontList().end()) {
				ImGui::BeginGroup();
				ImGui::Image((void*)assetsManager.GetTexture("fileIcon"), { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });
				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
					ImGui::SetDragDropPayload("FONT_PAYLOAD", asset.c_str(), asset.size() + 1, ImGuiCond_Once);
					ImGui::Text("Dragging: %s", asset.c_str());
					ImGui::EndDragDropSource();
				}
				/*ImGui::Text(asset.c_str());*/
				std::string trunStr = cutString(asset, thumbnailSize);
				ImGui::Text(trunStr.c_str());

				ImGui::EndGroup();

				currItemIndex++;
				if (currItemIndex % columns != 0) {
					ImGui::SameLine(); // Place the next item in the same row
				}
			}
		}
	}

	ImGui::NewLine();
	ImGui::Separator();

	ImGui::Text("Drag and drop an asset here to delete it:");
	ImGui::SameLine();
	ImGui::Button("Delete Zone");

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payloadTex = ImGui::AcceptDragDropPayload("TEXTURE_PAYLOAD")) {
			const char* assetName = (const char*)payloadTex->Data;
			if (assetName) {
				std::cout << "Deleted texture asset: " << assetName << std::endl;
				assetsManager.UnloadTexture(assetName);
				assetsManager.DeleteAssetFromJSON(assetName, "textureAssets");
			}
		}
		else if (const ImGuiPayload* payloadShdr = ImGui::AcceptDragDropPayload("SHADER_PAYLOAD")) {
			const char* assetName = (const char*)payloadShdr->Data;
			if (assetName) {
				std::cout << "Deleted shader asset: " << assetName << std::endl;
			}
		}
		else if (const ImGuiPayload* payloadAud = ImGui::AcceptDragDropPayload("AUDIO_PAYLOAD")) {
			const char* assetName = (const char*)payloadAud->Data;
			if (assetName) {
				std::cout << "Deleted shader asset: " << assetName << std::endl;
			}
		}
		else if (const ImGuiPayload* payloadFont = ImGui::AcceptDragDropPayload("FONT_PAYLOAD")) {
			const char* assetName = (const char*)payloadFont->Data;
			if (assetName) {
				std::cout << "Deleted font asset: " << assetName << std::endl;
			}
		}
		ImGui::EndDragDropTarget();
	}
}

void AssetBrowser::Cleanup() {}

std::string AssetBrowser::cutString(const std::string& str, float maxWidth) {
	const char* strEnd = "...";
	float width = ImGui::CalcTextSize(str.c_str()).x;
	float textWidth = ImGui::CalcTextSize(str.c_str()).x;

	if (textWidth <= maxWidth) {
		return str;
	}
	
	//str is too long and needs to be cut
	std::string finalStr = "";
	for (int i = 0; i < str.size(); i++) {
		finalStr += str[i];
		width = ImGui::CalcTextSize(finalStr.c_str()).x;
		if (width >= maxWidth) {
			finalStr += strEnd;
			break;
		}
	}

	return finalStr;
}