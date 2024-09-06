#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> 
#include <iostream>
#include "Program.h"
#include "logicgate.h"
#include "Button.h"
#include "LightBulb.h"

Program::Program() :
	nameTextureLocationMapping({
		{"notPath", "resources/Not.png"},
		{"orPath", "resources/Or.png"},
		{"andPath", "resources/And.png"},
		{"greenDotPath", "resources/GreenDotSmall.png"},
		{"linePath", "resources/Line.png"},
		{"unPressedButtonPath", "resources/UnPressedButton.png"},
		{"pressedButtonPath", "resources/PressedButton.png"},
		{"offLightBulbPath", "resources/OffLightBulb.png"},
		{"onLightBulbPath", "resources/OnLightBulb.png"},
		{"selectionBoxPath", "resources/SelectionBox.png"},
		{"xorPath", "resources/Xor.png"},
		{"clearPath", "resources/Clear.png"}
		}),
	proj(glm::ortho(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f)),
	spriteManager(nameTextureLocationMapping),
	spriteRenderer(720, 1280, "LogicGateSimulator", "../Renderer/res/shaders/basic.shader", spriteManager.m_textureSlots, spriteManager.m_numberOfTextures, proj),
	gui(spriteRenderer),
	inputHandler(spriteRenderer.getWindowPointer())
{
	spriteManager.BindTextures();
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	GLCall(glEnable(GL_BLEND));
	std::cout << glGetString(GL_VERSION) << std::endl;

	tempEntity = NULL;
	connectionComponent = NULL;
	selected = Selectable::NONE;

	holdingComponent = false;
	selecting = false;

	mouseStartXPos = 0;
	mouseStartYPos = 0;
	mouseEndXPos = 0;
	mouseEndYPos = 0;
}

void Program::Run()
{
	while (!spriteRenderer.WindowShouldClose())
	{
		ResetFrameState();

		HandleEntitySelectionFromMenu();

		HandleUserInteractionWithEntity();

		HandleMenuSelectedEntityPlacement();

		HandleEntitySelection();

		HandleCopyDeletePaste();

		UpdateEntitiesStates();

		RenderFrame();
	}
}

void Program::AddMenuSelectedEntity()
{
	if (tempEntity == NULL)
	{
		tempEntity = CreateEntity();
		selected = Selectable::NONE;
		entities.push_back(tempEntity);
	}
	else
		entities[entities.size() - 1]->MoveToPoint(inputHandler.mouseXPos, inputHandler.mouseYPos);
}

void Program::CopySelection()
{
	for (int i = 0; i < copiedEntities.size(); ++i)
	{
		delete copiedEntities[i];
	}
	copiedEntities.clear();

	// two map checks here
	std::map<Entity*, Entity*> copied;
	std::map<Entity*, bool> existsInSelectedEntites;
	for (int i = 0; i < selectedEntities.size(); ++i)
	{
		existsInSelectedEntites[selectedEntities[i]] = true;
	}

	for (int i = 0; i < selectedEntities.size(); ++i)
	{
		Entity* copiedEntity = selectedEntities[i]->Copy();
		copiedEntities.push_back(copiedEntity);

		copied[selectedEntities[i]] = copiedEntity;
	}

	for (auto& entityPair : copied)
	{
		for (int i = 0; i < entityPair.first->m_components.size(); ++i)
		{
			if (entityPair.first->m_components[i]->m_outPutComponenet || entityPair.first->m_components[i]->m_connectedTo == NULL)
				continue;

			if (copied[entityPair.first->m_components[i]->m_connectedTo->m_parentEntity])
			{
				Entity* referenceEntity = copied[entityPair.first->m_components[i]->m_connectedTo->m_parentEntity];
				ConnectionComponent* outputComponent = NULL;
				for (int j = 0; j < referenceEntity->m_components.size(); ++j)
				{
					if (referenceEntity->m_components[j]->m_outPutComponenet)
						outputComponent = referenceEntity->m_components[j];
				}
				entityPair.second->m_components[i]->m_connectedTo = outputComponent;
				entityPair.second->m_components[i]->m_connectedTo->m_connectedTo = entityPair.second->m_components[i];
			}
		}
	}
}

void Program::PasteEntities()
{
	float centerX = 0.0f;
	float centerY = 0.0f;

	for (int i = 0; i < copiedEntities.size(); ++i)
	{
		centerX += (copiedEntities[i]->m_sprite.verticies[0].position[0] + copiedEntities[i]->m_sprite.verticies[2].position[0]) / 2;
		centerY += (copiedEntities[i]->m_sprite.verticies[0].position[1] + copiedEntities[i]->m_sprite.verticies[2].position[1]) / 2;
	}

	centerX /= copiedEntities.size();
	centerY /= copiedEntities.size();

	float displacementX = inputHandler.mouseXPos - centerX;
	float displacementY = inputHandler.mouseYPos - centerY;

	for (int i = 0; i < copiedEntities.size(); ++i)
	{
		copiedEntities[i]->MoveAlongVector(displacementX, displacementY);
		entities.push_back(copiedEntities[i]);
	}
	copiedEntities.clear();
	selectedEntities.clear();
}

void Program::AddEntitiesToSelection()
{
	selectedEntities.clear();
	mouseEndXPos = inputHandler.mouseXPos;
	mouseEndYPos = inputHandler.mouseYPos;

	std::cout << mouseStartXPos << " " << mouseStartYPos << '\n';
	std::cout << mouseEndXPos << " " << mouseEndYPos << '\n';

	selecting = false;
	Entity selectionBox(spriteManager.CreateSprite(nameTextureLocationMapping["selectionBoxPath"], mouseStartXPos, mouseStartYPos, mouseEndXPos, mouseEndYPos, 1, false));

	for (int j = 0; j < 6; ++j)
	{
		verticies.push_back(selectionBox.m_sprite.verticies[j]);
	}

	for (int i = 0; i < entities.size(); ++i)
	{
		if (collisionManager.AreSpritesCollided(selectionBox.m_sprite, entities[i]->m_sprite))
		{
			selectedEntities.push_back(entities[i]);
			AddHighlightBoxToEntity(entities[i]);
		}
	}
	std::cout << selectedEntities.size();
}

void Program::AttachClickedComponent(int i)
{
	bool move = true;

	for (ConnectionComponent* component : entities[i]->m_components)
	{
		if (collisionManager.IsSpriteClicked(component->m_sprite, inputHandler.mouseXPos, inputHandler.mouseYPos, inputHandler.m_currentInputEvent))
		{
			move = false;
			if (connectionComponent == NULL)
			{
				std::cout << "pressed";
				bool output = false;
				if (component->m_connectedTo != NULL && !component->m_outPutComponenet)
				{
					component->m_connectedTo->m_connectedTo = NULL;
					output = true;
				}
				component->m_connectedTo = NULL;

				if (!output)
				{

				connectionComponent = component;
				holdingComponent = true;
				}
			}

			else if (component->m_parentEntity != connectionComponent->m_parentEntity && (connectionComponent->m_outPutComponenet && !component->m_outPutComponenet || !connectionComponent->m_outPutComponenet && component->m_outPutComponenet))
			{
				if (component->m_connectedTo != NULL && component->m_connectedTo->m_outPutComponenet)
					component->m_connectedTo->m_connectedTo = NULL;

				component->m_connectedTo = connectionComponent;
				connectionComponent->m_connectedTo = component;
				std::cout << "added to" << (int)((LogicGate*)component->m_parentEntity)->m_type << std::endl;
				connectionComponent = NULL;				
			}
			else
			{
				connectionComponent = NULL;
			}
			break;
		}
	}

	if (entities[i]->m_clickable)
	{
		if (entities[i]->m_components[0] == connectionComponent)
		{
			connectionComponent = NULL;
			entities[i]->m_state = !entities[i]->m_state;
			holdingComponent = false;
		}
	}

	if (move)
		collisionManager.m_clickedEntity = entities[i];
	
}

void Program::AddSpritesToVertexBuffer()
{
	// Draws the line connecting a component and the mouse cursor
	if (connectionComponent != NULL)
	{
		float compenent1XCenter =
			(connectionComponent->m_sprite.verticies[0].position[0] + connectionComponent->m_sprite.verticies[1].position[0]) / 2;

		float compenent1YCenter =
			(connectionComponent->m_sprite.verticies[0].position[1] + connectionComponent->m_sprite.verticies[1].position[1]) / 2 + 0.05f;

		Sprite blackLine = spriteManager.CreateSprite(nameTextureLocationMapping["linePath"], compenent1XCenter, compenent1YCenter, inputHandler.mouseXPos, inputHandler.mouseYPos, 0.1f);

		for (int j = 0; j < 6; ++j)
			verticies.push_back(blackLine.verticies[j]);
	}

	// Draws the lines between components
	// This was done in a separate loop so that they are drawn beneath the components and entities
	for (int i = 0; i < entities.size(); ++i)
	{
		for (const ConnectionComponent* connectionComponent : entities[i]->m_components)
		{
			if (connectionComponent->m_connectedTo != NULL)
			{

				float compenent1XCenter =
					(connectionComponent->m_sprite.verticies[0].position[0] + connectionComponent->m_sprite.verticies[1].position[0]) / 2;

				float compenent1YCenter =
					(connectionComponent->m_sprite.verticies[0].position[1] + connectionComponent->m_sprite.verticies[1].position[1]) / 2 + 0.05f;

				float compenent2XCenter =
					(connectionComponent->m_connectedTo->m_sprite.verticies[0].position[0] + connectionComponent->m_connectedTo->m_sprite.verticies[1].position[0]) / 2;
				float compenent2YCenter =
					(connectionComponent->m_connectedTo->m_sprite.verticies[0].position[1] + connectionComponent->m_connectedTo->m_sprite.verticies[1].position[1]) / 2 + 0.05f;

				Sprite blackLine = spriteManager.CreateSprite(nameTextureLocationMapping["linePath"], compenent1XCenter, compenent1YCenter, compenent2XCenter, compenent2YCenter, 0.1f);

				for (int j = 0; j < 6; ++j)
					verticies.push_back(blackLine.verticies[j]);
			}
		}
	}

	for (int i = 0; i < entities.size(); ++i)
	{
		// Draws all the Entities
		for (int j = 0; j < 6; ++j)
			verticies.push_back(entities[i]->m_sprite.verticies[j]);

		for (const ConnectionComponent* connectionComponent : entities[i]->m_components)
		{
			for (int j = 0; j < 6; ++j)
				verticies.push_back(connectionComponent->m_sprite.verticies[j]);
		}
	}
}

void Program::AddHighlightBoxToEntity(Entity* entity)
{
	Entity selectionBox(spriteManager.CreateSprite(nameTextureLocationMapping["selectionBoxPath"], entity->m_sprite.m_xPos, entity->m_sprite.m_yPos, 1));
	for (int j = 0; j < 6; ++j)
	{
		verticies.push_back(selectionBox.m_sprite.verticies[j]);
	}
}

void Program::DeleteEntity(Entity* entity)
{
	for (auto component : entity->m_components)
	{
		if(component->m_connectedTo != NULL)
			(component->m_connectedTo)->m_connectedTo = NULL;
	}

	auto it = std::find(selectedEntities.begin(), selectedEntities.end(),
		entity);
	if(it != selectedEntities.end())
		selectedEntities.erase(it);

	it = std::find(entities.begin(), entities.end(),
		entity);
	if (it != entities.end())
		entities.erase(it);

	delete entity;
}

void Program::ResetFrameState()
{
	holdingComponent = false;
	spriteRenderer.Clear();

	spriteRenderer.PollEvents();
	gui.BeginNewFrame();

	glm::mat4 zoomMatrix = glm::mat4(1.0f);
	if (inputHandler.m_zoomUsed && (inputHandler.m_prevZoomLevel - inputHandler.m_zoomLevel != 0))
	{
		float aspectRatio = 16.0f / 9.0f; // Assuming 16:9 aspect ratio
		float newWidth = 16.0f * inputHandler.m_zoomLevel;
		float newHeight = 9.0f * inputHandler.m_zoomLevel;

		// Store the mouse position before updating the projection
		glm::vec2 oldMousePos(inputHandler.mouseXPos, inputHandler.mouseYPos);

		// Calculate new projection matrix
		proj = glm::ortho(-newWidth / 2.0f, newWidth / 2.0f, -newHeight / 2.0f, newHeight / 2.0f, -1.0f, 1.0f);

		// Calculate the offset to center the zoom around the cursor
		glm::vec2 zoomCenter = oldMousePos;
		glm::vec2 newCenter(0.0f, 0.0f); // Center of the new view

		// Translate the projection to zoom around the cursor
		if (inputHandler.m_prevZoomLevel - inputHandler.m_zoomLevel > 0)
		{
			prevZoomXCenter = zoomCenter.x;
			prevZoomYCenter = zoomCenter.y;
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(newCenter - zoomCenter, 0.0f));
			proj = proj * translation;
		}
		else
		{
			zoomCenter.x = prevZoomXCenter;
			zoomCenter.y = prevZoomYCenter;
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(newCenter - zoomCenter, 0.0f));
			proj = proj * translation;
		}
	}

	spriteRenderer.UpdateProjectionMatrix(proj);

	inputHandler.UpdateInput(proj);
	verticies.clear();
}

void Program::HandleCopyDeletePaste()
{
	if (inputHandler.m_currentInputEvent == InputEvents::COPYING)
			CopySelection();

	else if (inputHandler.m_currentInputEvent == InputEvents::PASTING && !copiedEntities.empty())
		PasteEntities();

	else if (inputHandler.m_currentInputEvent == InputEvents::DELETING)
	{
		int size = selectedEntities.size();
		for (int i = size-1; i >= 0; --i)
		{
			DeleteEntity(selectedEntities[i]);
		}
	}
}

void Program::HandleEntitySelectionFromMenu()
{
	selected = gui.DrawMenu(spriteManager, nameTextureLocationMapping, inputHandler);

	if (inputHandler.m_currentInputEvent == InputEvents::MOUSE_DRAG && selected != Selectable::NONE)
		AddMenuSelectedEntity();
	else
		selected = Selectable::NONE;
}

void Program::HandleMenuSelectedEntityPlacement()
{

	if (tempEntity != NULL && inputHandler.m_currentInputEvent == InputEvents::MOUSE_RELEASE && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AnyWindow))
	{
		delete tempEntity;
		tempEntity = NULL;
		entities.pop_back();
	}

	else if (tempEntity != NULL && inputHandler.m_currentInputEvent == InputEvents::MOUSE_RELEASE)
		tempEntity = NULL;
}

void Program::HandleEntitySelection()
{
	if (inputHandler.m_currentInputEvent == InputEvents::MOUSE_DRAG && tempEntity == NULL
		&& !selecting && collisionManager.m_clickedEntity == NULL && connectionComponent == NULL)
	{
		mouseStartXPos = inputHandler.mouseXPos;
		mouseStartYPos = inputHandler.mouseYPos;
		selecting = true;
	}
	else if (inputHandler.m_currentInputEvent == InputEvents::LEFT_MOUSE_CLICKED && !IsEntityInSelectedEntites(collisionManager.m_clickedEntity))
	{
		selecting = false;
		selectedEntities.clear();
	}

	if (inputHandler.m_currentInputEvent == InputEvents::MOUSE_RELEASE && selecting && !collisionManager.IsEntityBeingDragged(inputHandler.m_currentInputEvent))
		AddEntitiesToSelection();
	else if (selecting && !collisionManager.IsEntityBeingDragged(inputHandler.m_currentInputEvent))
	{
		mouseEndXPos = inputHandler.mouseXPos;
		mouseEndYPos = inputHandler.mouseYPos;

		Entity selectionBox(spriteManager.CreateSprite(nameTextureLocationMapping["selectionBoxPath"], mouseStartXPos, mouseStartYPos, mouseEndXPos, mouseEndYPos, 1, false));

		for (int j = 0; j < 6; ++j)
		{
			verticies.push_back(selectionBox.m_sprite.verticies[j]);
		}
	}

	if (selectedEntities.size() != 0)
	{
		for (int i = 0; i < selectedEntities.size(); ++i)
			AddHighlightBoxToEntity(selectedEntities[i]);
	}
}

void Program::HandleUserInteractionWithEntity()
{
	if (collisionManager.IsEntityBeingDragged(inputHandler.m_currentInputEvent) && selectedEntities.size() > 0
		&& IsEntityInSelectedEntites(collisionManager.m_clickedEntity))
	{
		float centerX = 0.0f;
		float centerY = 0.0f;

		for (int i = 0; i < selectedEntities.size(); ++i)
		{
			centerX += (selectedEntities[i]->m_sprite.verticies[0].position[0] + selectedEntities[i]->m_sprite.verticies[2].position[0]) / 2;
			centerY += (selectedEntities[i]->m_sprite.verticies[0].position[1] + selectedEntities[i]->m_sprite.verticies[2].position[1]) / 2;
		}

		centerX /= selectedEntities.size();
		centerY /= selectedEntities.size();

		float displacementX = inputHandler.mouseXPos - centerX;
		float displacementY = inputHandler.mouseYPos - centerY;

		for (int i = 0; i < selectedEntities.size(); ++i)
		{
			selectedEntities[i]->MoveAlongVector(displacementX, displacementY);
		}
	}
	else if (collisionManager.IsEntityBeingDragged(inputHandler.m_currentInputEvent))
	{
		collisionManager.m_clickedEntity->MoveToPoint(inputHandler.mouseXPos, inputHandler.mouseYPos);
	}
	else
	{
		for (int i = 0; i < entities.size(); ++i)
		{
			if (collisionManager.IsSpriteClicked(entities[i]->m_sprite, inputHandler.mouseXPos, inputHandler.mouseYPos, inputHandler.m_currentInputEvent))
			{
				AttachClickedComponent(i);
				break;
			}
		}
	}

	if (connectionComponent != NULL && inputHandler.m_currentInputEvent == InputEvents::LEFT_MOUSE_CLICKED && !holdingComponent)
		connectionComponent = NULL;
}

void Program::UpdateEntitiesStates()
{
	for (int i = 0; i < entities.size(); ++i)
		entities[i]->Reset();
	for (int i = 0; i < entities.size(); ++i)
		entities[i]->Update();
}

bool Program::IsEntityInSelectedEntites(Entity* entity)
{
	return std::find(selectedEntities.begin(), selectedEntities.end(), entity) != selectedEntities.end();
}

void Program::RenderFrame()
{
	AddSpritesToVertexBuffer();

	if (!verticies.empty())
		spriteRenderer.DrawSprite(verticies);

	gui.Render();
	glfwSwapBuffers(spriteRenderer.getWindowPointer());
}

Entity* Program::CreateEntity()
{
	float buttonSize = 0.8f;
	Sprite smDot = spriteManager.CreateSprite(nameTextureLocationMapping["greenDotPath"], 2, 1, 0.090f);

	switch (selected)
	{
	case Selectable::NOT:
		return new LogicGate(LogicGatesTypes::NOT, spriteManager.CreateSprite(nameTextureLocationMapping["notPath"], inputHandler.mouseXPos, inputHandler.mouseYPos, 1.0f), smDot);
	case Selectable::OR:
		return new LogicGate(LogicGatesTypes::OR, spriteManager.CreateSprite(nameTextureLocationMapping["orPath"], inputHandler.mouseXPos, inputHandler.mouseYPos, 1.0f), smDot);
	case Selectable::XOR:
		return new LogicGate(LogicGatesTypes::XOR, spriteManager.CreateSprite(nameTextureLocationMapping["xorPath"], inputHandler.mouseXPos, inputHandler.mouseYPos, 1.0f), smDot);
	case Selectable::AND:
		return new LogicGate(LogicGatesTypes::AND, spriteManager.CreateSprite(nameTextureLocationMapping["andPath"], inputHandler.mouseXPos, inputHandler.mouseYPos, 1.0f), smDot);
	case Selectable::BULB:
		return new LightBulb(spriteManager.CreateSprite(nameTextureLocationMapping["offLightBulbPath"], inputHandler.mouseXPos, inputHandler.mouseYPos, 1), smDot,
			spriteManager.GetTextureSlot(nameTextureLocationMapping["onLightBulbPath"]), spriteManager.GetTextureSlot(nameTextureLocationMapping["offLightBulbPath"]));
	case Selectable::BUTTON:
		return new Button(spriteManager.CreateSprite(nameTextureLocationMapping["unPressedButtonPath"], inputHandler.mouseXPos, inputHandler.mouseYPos, 1.0), smDot,
			spriteManager.CreateSprite(nameTextureLocationMapping["clearPath"], 0.0f, 0.0f, 0.25f * buttonSize), spriteManager.GetTextureSlot(nameTextureLocationMapping["pressedButtonPath"]), spriteManager.GetTextureSlot(nameTextureLocationMapping["unPressedButtonPath"]));
	default:
		return NULL;
	}
}