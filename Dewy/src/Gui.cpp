#include "Gui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

Selectable Gui::DrawMenu(SpriteManager& spriteManager, std::map<std::string, std::string>& nameTextureLocationMapping, InputHandler inputHandler)
{
	Selectable selected = Selectable::NONE;

	ImGui::Begin("Object Menu", NULL, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::PushStyleColor(ImGuiCol_Button, { 1,1,1,1 });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0,0.8,0,1 });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0,0.8,0,1 });

	std::vector<std::string> textureLocation = { "notPath", "orPath", "andPath", "xorPath", "offLightBulbPath", "unPressedButtonPath" };
	std::vector<std::string> text = { "Not Gate", "Or Gate", "And Gate", "Xor Gate", "Light Bulb", "Button" };

	for (int i = 0; i < textureLocation.size(); ++i)
	{
		ImGui::BeginGroup();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), (text[i]).c_str());
		if (ImGui::ImageButton((text[i]).c_str(), (void*)spriteManager.GetTextureSlot(nameTextureLocationMapping[textureLocation[i]]), {100,50}, {0,1}, {1,0}, {0,0,0,1})
			|| ImGui::IsItemActive())
		{
			selected = (Selectable)(i+1);
		}
		ImGui::EndGroup();
		ImGui::SameLine();
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::End();

	return selected;
}

