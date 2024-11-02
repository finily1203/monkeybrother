#include "GUIConsole.h"

//Variables for Console
std::vector<std::string> Console::items;
bool Console::autoScroll = true;
bool Console::autoDelete = true;
float Console::lastScrollY;
Console* Console::instance = nullptr;
std::ostringstream Console::currentLog;

Console& Console::GetLog() {
	if (instance == nullptr) {
		instance = new Console();
	}
	return *instance;
}

void Console::Update(const char* title) {
	GetLog().DrawImpl(title);
}

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
		items.clear();
	if (copy)
		ImGui::LogToClipboard();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
	for (const auto& item : items)
		ImGui::TextUnformatted(item.c_str()); //Render log messages
	ImGui::PopStyleVar();

	// Auto-scroll logic
	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		autoScroll = true;
	if (ImGui::GetScrollY() < lastScrollY)
		autoScroll = false;
	lastScrollY = ImGui::GetScrollY();

	if (autoScroll && ImGui::GetScrollY() < ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	static char inputBuffer[256] = "";
	//When enter key is pressed
	if (ImGui::InputText("Input", inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
		if (inputBuffer[0]) {
			*this << "> " << inputBuffer << std::endl;
			// Parse and execute the command
			*this << "Command executed: " << inputBuffer << std::endl;
		}
		inputBuffer[0] = 0; // Clear the input buffer
	}

	ImGui::SameLine();
	//When submit button is clicked
	if (ImGui::Button("Submit")) {
		if (inputBuffer[0]) {
			*this << "> " << inputBuffer << std::endl;

			*this << "Command executed: " << inputBuffer << std::endl;
			inputBuffer[0] = 0; // Clear the input buffer
		}
	}

	ImGui::End();
}

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
}