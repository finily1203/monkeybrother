#include "Debug.h"


DebugSystem::DebugSystem() : io{ nullptr } {}

DebugSystem::~DebugSystem() {}

void DebugSystem::initialise() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	//font1 = io.Fonts->AddFontFromFileTTF("Assets/yourfont.ttf", 20);
	//font2 = io.Fonts->AddFontFromFileTTF("Assets/yourfont.ttf", 15);
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Separator] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); //outer border of table
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.8f, 0.8f, 0.8f, 1.0f); //inner border of table
	style.SeparatorTextBorderSize = 3.0f;
	style.TabBorderSize = 3.0f;


	ImGui_ImplGlfw_InitForOpenGL(GLFWFunctions::pWindow, true);

	ImGui_ImplOpenGL3_Init("#version 130");
}

void DebugSystem::update() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//ImGui::ShowDemoWindow();
	if (ImGui::CollapsingHeader("Performance Data")) {
		static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | /*ImGuiTableFlags_ScrollY |*/
			ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;

		ImVec2 outer_size = ImVec2(0.0f, ImGui::CalcTextSize("A").x * 5.5f);
		//ImVec2 outer_size = ImVec2(0.0f, 50.f);

		//ImGui::PushFont(font1);
		ImGui::Text("FPS: %.1f", GLFWFunctions::fps);
		//ImGui::PopFont();

		if (ImGui::BeginTable("Debug", 3, flags, outer_size)) {

			ImGui::TableSetupColumn("Systems");
			ImGui::TableSetupColumn("Game Loop Time (ms)");
			ImGui::TableSetupColumn("Game Loop %");
			ImGui::TableHeadersRow();
			/*for (const auto& [systemName, systemTime] : systemTimes) {

			}*/
			for (int i{ 0 }; i < 3; i++) {
				ImGui::TableNextRow();
				ImGui::TableNextColumn();
				ImGui::Text(systems[i]);
				ImGui::TableNextColumn();
				ImGui::Text("-");
				ImGui::TableNextColumn();
				ImGui::Text("-");
			}

			ImGui::EndTable();
		}

	}

	if (ImGui::CollapsingHeader("Input Data")) {
		ImGui::SeparatorText("Mouse Coordinates");
		if (ImGui::IsMousePosValid())
			ImGui::Text("Mouse pos: (%g, %g)", io->MousePos.x, io->MousePos.y);
		else
			ImGui::Text("Mouse pos: <INVALID>");
		/*ImGui::Text("Mouse down:");
		for (int i = 0; i < (int)(sizeof(io->MouseDown) / sizeof(*(io->MouseDown))); i++) {
			if (ImGui::IsMouseDown(i)) {
				ImGui::SameLine();
				ImGui::Text("b%d (%.02f secs)", i, io->MouseDownDuration[i]);
			}
		}*/

		ImGui::SeparatorText("Mouse/Keys pressed:\n");
		ImGuiKey startKey = (ImGuiKey)0;

		//Check that key exist in ImGuiKey data (includes legacy and modern keys)
		for (ImGuiKey key = startKey; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
		{
			//Don't render if key is typed legacy OR if the key is not pressed (avoid duplication)
			if (LegacyKeyDuplicationCheck(key) //Check if key is the legacy version
				|| !ImGui::IsKeyDown(key)) //Check if key is pressed
				continue; //iterates the next key in ImGuiKey
			ImGui::Text("\"%s\"", ImGui::GetKeyName(key));
			ImGui::SameLine();
		}


	}

	//Rendering of UI
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(GLFWFunctions::pWindow, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void DebugSystem::cleanup() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void DebugSystem::startLoop() {
	loopStartTime = glfwGetTime();
	systemTimes.clear();
}

void DebugSystem::startSystemTiming(const std::string& systemName) {
	systemTimes[systemName] -= glfwGetTime() - loopStartTime;
}

void DebugSystem::endSystemTiming(const std::string& systemName) {
	systemTimes[systemName] += glfwGetTime() - loopStartTime;
}

void DebugSystem::endLoop() {
	totalLoopTime = GLFWFunctions::delta_time;
}

static bool LegacyKeyDuplicationCheck(ImGuiKey key) {
	//Check key code within 0 and 512 due to old ImGui key management (if found means its a legacy key)
	return key >= 0 && key < 512
		&& ImGui::GetIO().KeyMap[key] != -1; //Check if legacy key is mapped in ImGui key map
}

//void Debug::Run(double& fps) {
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplGlfw_NewFrame();
//	ImGui::NewFrame();
//
//	//ImGui::ShowDemoWindow();
//	if (ImGui::CollapsingHeader("Performance Data")) {
//		static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | /*ImGuiTableFlags_ScrollY |*/
//			ImGuiTableFlags_ContextMenuInBody | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_NoHostExtendX;
//
//		ImVec2 outer_size = ImVec2(0.0f, ImGui::CalcTextSize("A").x * 5.5f);
//		//ImVec2 outer_size = ImVec2(0.0f, 50.f);
//
//		//ImGui::PushFont(font1);
//		ImGui::Text("FPS: %.1f", fps);
//		//ImGui::PopFont();
//
//		if (ImGui::BeginTable("Debug", 3, flags, outer_size)) {
//
//			ImGui::TableSetupColumn("Systems");
//			ImGui::TableSetupColumn("Game Loop Time (ms)");
//			ImGui::TableSetupColumn("Game Loop %");
//			ImGui::TableHeadersRow();
//
//			for (int i{ 0 }; i < 3; i++) {
//				ImGui::TableNextRow();
//				ImGui::TableNextColumn();
//				ImGui::Text(systems[i]);
//				ImGui::TableNextColumn();
//				ImGui::Text("-");
//				ImGui::TableNextColumn();
//				ImGui::Text("-");
//			}
//
//			ImGui::EndTable();
//		}
//
//	}
//
//	if (ImGui::CollapsingHeader("Input Data")) {
//		ImGui::SeparatorText("Mouse Coordinates");
//		if (ImGui::IsMousePosValid())
//			ImGui::Text("Mouse pos: (%g, %g)", io->MousePos.x, io->MousePos.y);
//		else
//			ImGui::Text("Mouse pos: <INVALID>");
//		/*ImGui::Text("Mouse down:");
//		for (int i = 0; i < (int)(sizeof(io->MouseDown) / sizeof(*(io->MouseDown))); i++) {
//			if (ImGui::IsMouseDown(i)) {
//				ImGui::SameLine();
//				ImGui::Text("b%d (%.02f secs)", i, io->MouseDownDuration[i]);
//			}
//		}*/
//			
//		ImGui::SeparatorText("Mouse/Keys pressed:\n");         
//		ImGuiKey startKey = (ImGuiKey)0;
//
//		//Check that key exist in ImGuiKey data (includes legacy and modern keys)
//		for (ImGuiKey key = startKey; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1))
//		{ 
//			//Don't render if key is typed legacy OR if the key is not pressed (avoid duplication)
//			if (LegacyKeyDuplicationCheck(key) //Check if key is the legacy version
//				|| !ImGui::IsKeyDown(key)) //Check if key is pressed
//				continue; //iterates the next key in ImGuiKey
//			ImGui::Text( "\"%s\"", ImGui::GetKeyName(key)); 
//			ImGui::SameLine();
//		}
//
//		
//	}
//
//}  //Run

//void Debug::Display() {
//	ImGui::Render();
//	int display_w, display_h;
//	glfwGetFramebufferSize(GLFWFunctions::pWindow, &display_w, &display_h);
//	glViewport(0, 0, display_w, display_h);
//	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
//	glClear(GL_COLOR_BUFFER_BIT);
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//} //Display
