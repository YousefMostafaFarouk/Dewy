#include "Gui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdint>

Gui::Gui(SpriteRender& spriteRenderer)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiStyle& style = ImGui::GetStyle();
	ioptr = &ImGui::GetIO(); (void)(ioptr);
	(*ioptr).ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	(*ioptr).ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.WindowBorderSize = 0;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(spriteRenderer.getWindowPointer(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void Gui::BeginNewFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
}

void Gui::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if ((*ioptr).ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

Selectable Gui::DrawMenu(
	SpriteManager& spriteManager,
	std::map<std::string, std::string>& nameTextureLocationMapping,
	const std::string& circuitFileStatus,
	bool circuitFileStatusIsError,
	CircuitFileRequest& circuitFileRequest)
{
	Selectable selected = Selectable::NONE;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, 185.0f), ImGuiCond_Always);
	ImGui::Begin(
		"Object Menu",
		NULL,
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings);
	ImGui::PushStyleColor(ImGuiCol_Button, { 1,1,1,1 });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0.8,0,1 });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0.8,0,1 });

	std::vector<std::string> textureLocation = { "notPath", "orPath", "andPath", "xorPath", "offLightBulbPath", "unPressedButtonPath" };
	std::vector<std::string> text = { "Not Gate", "Or Gate", "And Gate", "Xor Gate", "Light Bulb", "Button" };

	for (std::size_t i = 0; i < textureLocation.size(); ++i)
	{
		ImGui::BeginGroup();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (text[i]).c_str());
		const auto textureId = reinterpret_cast<void*>(static_cast<std::intptr_t>(
			spriteManager.GetTextureRendererId(nameTextureLocationMapping[textureLocation[i]])));
		if (ImGui::ImageButton((text[i]).c_str(), textureId, {100,50}, {0,1}, {1,0}, {0,0,0,1})
			|| ImGui::IsItemActive())
		{
			selected = static_cast<Selectable>(i + 1);
		}
		ImGui::EndGroup();
		ImGui::SameLine();
	}
	ImGui::PopStyleColor(3);

	// Add Help Button
	if (ImGui::Button("Help"))
	{
		ImGui::OpenPopup("HelpPopup");
	}

	if (ImGui::BeginPopup("HelpPopup"))
	{
		ImGui::Text("Instructions:");
		ImGui::BulletText("To select an element click and drag your mouse over them.");
		ImGui::BulletText("To copy selected elements press Ctrl + C.");
		ImGui::BulletText("To paste selected elements press Ctrl + V to paste the elements to the mouse location.");
		ImGui::BulletText("To delete selected elements press Backspace.");
		ImGui::BulletText("You can drag selected elements by holding left click on any of them and moving your mouse.");
		ImGui::BulletText("If you click on an input node it will disconnect it from any output nodes that it is connected to.");
		ImGui::EndPopup();
	}

	ImGui::Separator();
	ImGui::TextUnformatted("Circuit file");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(300.0f);
	ImGui::InputText("##CircuitFilePath", circuitFilePath, sizeof(circuitFilePath));
	ImGui::SameLine();

	if (ImGui::Button("Save Circuit"))
		circuitFileRequest.action = CircuitFileAction::SAVE;
	ImGui::SameLine();
	if (ImGui::Button("Load Circuit"))
		circuitFileRequest.action = CircuitFileAction::LOAD;

	if (!circuitFileStatus.empty())
	{
		const ImVec4 color = circuitFileStatusIsError
			? ImVec4(1.0f, 0.35f, 0.35f, 1.0f)
			: ImVec4(0.35f, 0.9f, 0.45f, 1.0f);
		ImGui::SameLine();
		ImGui::TextColored(color, "%s", circuitFileStatus.c_str());
	}

	circuitFileRequest.path = circuitFilePath;
	ImGui::End();

	return selected;
}
