#pragma once
#include <map>
#include <string>
#include "SpriteRenderer.h"
#include "Selectable.h"
#include "SpriteManager.h"
#include "imgui.h"

enum class CircuitFileAction
{
    NONE,
    SAVE,
    LOAD
};

struct CircuitFileRequest
{
    CircuitFileAction action{ CircuitFileAction::NONE };
    std::string path;
};

class Gui
{
public:
    Gui(SpriteRender& spriteRenderer);
    void BeginNewFrame();
    void Render();
    Selectable DrawMenu(
        SpriteManager& spriteManager,
        std::map<std::string, std::string>& nameTextureLocationMapping,
        const std::string& circuitFileStatus,
        bool circuitFileStatusIsError,
        CircuitFileRequest& circuitFileRequest);
private:
    ImGuiIO* ioptr;
    char circuitFilePath[260]{ "circuits/circuit.json" };
};
