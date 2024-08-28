#pragma once
#include <map>
#include "SpriteRenderer.h"
#include "Selectable.h"
#include "SpriteManager.h"
#include "imgui.h"
#include "InputHandler.h"

class Gui
{
public:
    Gui(SpriteRender& spriteRenderer);
    void BeginNewFrame();
    void Render();
    Selectable DrawMenu(SpriteManager& spriteManager, std::map<std::string, std::string>& nameTextureLocationMapping, InputHandler inputHandler);
private:
    ImGuiIO* ioptr;
};

