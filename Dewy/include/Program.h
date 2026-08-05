#pragma once
#include "Gui.h"
#include "Entity.h"
#include "InputHandler.h"
#include "CollisionManager.h"
#include "CircuitSerializer.h"
#include "TopologicalOrder.h"

class Program
{
public:
    Program();
    ~Program();
    void Run();
    void LoadCircuit(const std::filesystem::path& path);

private:
    std::vector<Entity*> entities;
    std::vector<Entity*> copiedEntities;
    std::vector<Entity*> selectedEntities;
    std::vector<Vertex> verticies;
    TopologicalOrder<Entity*> circuitOrder;

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
    float prevZoomXCenter = 0.0f, prevZoomYCenter = 0.0f;
    bool selecting = false;
    std::string circuitFileStatus;
    bool circuitFileStatusIsError = false;

    void AddMenuSelectedEntity();
    void CopySelection();
    void PasteEntities();
    void AddEntitiesToSelection();
    void AttachClickedComponent(int i);
    void DisconnectComponent(ConnectionComponent* component);
    void RestoreConnection(ConnectionComponent* output, ConnectionComponent* input);
    bool TryConnectComponents(ConnectionComponent* first, ConnectionComponent* second);
    void AddSpritesToVertexBuffer();
    void AddHighlightBoxToEntity(Entity* entity);
    void DeleteEntity(Entity* entity);
    void ResetFrameState();
    void HandleCopyDeletePaste();
    void HandleCircuitFileAction(const CircuitFileRequest& request);
    void HandleEntitySelectionFromMenu();
    void HandleMenuSelectedEntityPlacement();
    void HandleEntitySelection();
    void HandleUserInteractionWithEntity();
    void UpdateEntitiesStates();
    bool IsEntityInSelectedEntites(Entity* entity);
    void RenderFrame();
    Entity* CreateEntity();
    Entity* CreateEntity(Selectable entityType, float xPosition, float yPosition);
    CircuitDocument BuildCircuitDocument() const;
    void LoadCircuitDocument(const CircuitDocument& document);
    void ClearCircuit();
};
