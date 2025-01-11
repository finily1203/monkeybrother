/*
All content @ 2024 DigiPen Institute of Technology Singapore, all rights reserved.
@author :  Lew Zong Han Owen (z.lew)
@team   :  MonkeHood
@course :  CSD2401
@file   :  GUIConsole.cpp
@brief  :  This file contains the function definition of ImGui console window

*Lew Zong Han Owen (z.lew) :
		- Integrated ImGui console to allow an alternative channel to output debug information

*Ian Loi (ian.loi) :
		- Integrated serialization & deserialization functions to initialize variables and save from and to
		  json file

File Contributions: Lew Zong Han Owen (80%)
					Ian Loi           (20%)

/*_______________________________________________________________________________________________________________*/
#include "GUIConsole.h"

//Variables for Console
size_t Console::MAX_LOGS;
std::vector<std::string>* Console::items;
bool Console::autoScroll;
bool Console::autoDelete;
float Console::lastScrollY;
Console* Console::instance = nullptr;
std::ostringstream Console::currentLog;

Console::Console() {
    LoadConsoleConfigFromJSON(FilePathManager::GetIMGUIConsoleJSONPath());
    if (!items) {
        items = new std::vector<std::string>();
    }
    items->clear();

}

Console& Console::GetLog() {
	if (instance == nullptr) {
		instance = new Console();
	}
	return *instance;
}

void Console::Update(const char* title) {
	GetLog().DrawImpl(title);
}

void Console::Cleanup() {
    if (instance) {

        delete items;
        items = nullptr;
        currentLog.str("");
        currentLog.clear();
        delete instance;
        instance = nullptr;
    }
}

Console::~Console() {

}
//Implements the ImGui console window drawing and interaction logic
void Console::DrawImpl(const char* title) {
    ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(title)) {
        ImGui::End();
        return;
    }

    // Options menu
    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &autoScroll);
        ImGui::Checkbox("Auto Delete", &autoDelete);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::Separator();

    // Reserve enough left-over height for 1 separator + 1 input text
    const float heightReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -heightReserve), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (clear)
        items->clear();
    if (copy)
        ImGui::LogToClipboard();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
    for (const auto& item : *items)
        ImGui::TextUnformatted(item.c_str()); // Render log messages
    ImGui::PopStyleVar();

    // Auto-scroll logic - respecting checkbox state
    const float scrollThreshold = 20.0f;
    const bool isNearBottom = ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - scrollThreshold;

    // Only auto-scroll if enabled and we're near the bottom
    if (autoScroll && isNearBottom)
    {
        ImGui::SetScrollHereY(1.0f);
    }

    // Check for manual scrolling
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) || ImGui::GetIO().MouseWheel != 0.0f)
    {
        // Only update autoScroll if it's currently enabled
        if (autoScroll && !isNearBottom)
        {
            autoScroll = false;
        }
    }

    lastScrollY = ImGui::GetScrollY();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    static char inputBuffer[256] = "";
    bool reclaimFocus = false;

    //When enter key is pressed
    if (ImGui::InputText("Input", inputBuffer, IM_ARRAYSIZE(inputBuffer),
        ImGuiInputTextFlags_EnterReturnsTrue))
    {
        if (inputBuffer[0]) {
            *this << "> " << inputBuffer << std::endl;
            // Parse and execute the command
            *this << "Command executed: " << inputBuffer << std::endl;
            inputBuffer[0] = 0; // Clear the input buffer
            reclaimFocus = true;

            // Don't force auto-scroll, only scroll if it's enabled
            if (autoScroll) {
                ImGui::SetScrollHereY(1.0f);
            }
        }
    }

    // Auto-focus on the input box
    ImGui::SetItemDefaultFocus();
    if (reclaimFocus)
        ImGui::SetKeyboardFocusHere(-1);

    ImGui::SameLine();
    //When submit button is clicked
    if (ImGui::Button("Submit")) {
        if (inputBuffer[0]) {
            *this << "> " << inputBuffer << std::endl;
            *this << "Command executed: " << inputBuffer << std::endl;
            inputBuffer[0] = 0; // Clear the input buffer

            // Don't force auto-scroll, only scroll if it's enabled
            if (autoScroll) {
                ImGui::SetScrollHereY(1.0f);
            }
        }
    }

    ImGui::End();
}
//Load console configuration from JSON file
void Console::LoadConsoleConfigFromJSON(std::string const& filename)
{
	JSONSerializer serializer;

	// checks if the JSON file can be opened
	if (!serializer.Open(filename))
	{
		Console::GetLog() << "Error: could not open file " << filename << std::endl;
		return;
	}

	// retrieve the JSON object from the JSON file
	nlohmann::json currentObj = serializer.GetJSONObject();

	// read all of the data from the JSON object, assign every read
	// data to every elements that needs to be initialized
	serializer.ReadUnsignedLongLong(MAX_LOGS, "GUIConsole.maxLogs");
	serializer.ReadFloat(lastScrollY, "GUIConsole.lastScrollY");
    serializer.ReadBool(autoScroll, "GUIConsole.autoScroll");
    serializer.ReadBool(autoDelete, "GUIConsole.autoDelete");
}