#pragma once
#include "Gui.h"
#include "Entity.h"
#include "InputHandler.h"
#include "CollisionManager.h"

class Program
{
public:
    Program();
    void Run();

private:
    std::vector<Entity*> entities;
    std::vector<Entity*> copiedEntities;
    std::vector<Entity*> selectedEntities;
    std::vector<Vertex> verticies;

    std::map<std::string, std::string> nameTextureLocationMapping;
    glm::mat4 proj;
    Selectable selected;

    SpriteManager spriteManager;
    SpriteRender spriteRenderer;
    InputHandler inputHandler;
    CollisionManager collisionManager;

    Gui gui;

    Entity* tempEntity = NULL;
    ConnectionComponent* connectionComponent = NULL;

    bool holdingComponent = false;
    float mouseStartXPos = 0.0f, mouseStartYPos = 0.0f, mouseEndXPos = 0.0f, mouseEndYPos = 0.0f;
    bool selecting = false;

    void AddMenuSelectedEntity();
    void CopySelection();
    void PasteEntities();
    void AddEntitiesToSelection();
    void AttachClickedComponent(int i);
    void AddSpritesToVertexBuffer();
    void AddHighlightBoxToEntity(Entity* entity);
    void DeleteEntity(Entity* entity);
    void ResetFrameState();
    void HandleCopyDeletePaste();
    void HandleEntitySelectionFromMenu();
    void HandleMenuSelectedEntityPlacement();
    void HandleEntitySelection();
    void HandleUserInteractionWithEntity();
    void UpdateEntitiesStates();
    void RenderFrame();
    Entity* CreateEntity();
};

