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
	spriteRenderer(720, 1280, "LogicGateSimulator", "../Renderer/res/shaders/basic.shader", spriteManager.textureSlots, spriteManager.numberOfTextures, proj),
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
		for (int i = 0; i < entityPair.first->components.size(); ++i)
		{
			if (entityPair.first->components[i]->outPutComponenet || entityPair.first->components[i]->connectedTo == NULL)
				continue;

			if (copied[entityPair.first->components[i]->connectedTo->parentEntity])
			{
				Entity* referenceEntity = copied[entityPair.first->components[i]->connectedTo->parentEntity];
				ConnectionComponent* outputComponent = NULL;
				for (int j = 0; j < referenceEntity->components.size(); ++j)
				{
					if (referenceEntity->components[j]->outPutComponenet)
						outputComponent = referenceEntity->components[j];
				}
				entityPair.second->components[i]->connectedTo = outputComponent;
				entityPair.second->components[i]->connectedTo->connectedTo = entityPair.second->components[i];
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
		centerX += (copiedEntities[i]->sprite.verticies[0].position[0] + copiedEntities[i]->sprite.verticies[2].position[0]) / 2;
		centerY += (copiedEntities[i]->sprite.verticies[0].position[1] + copiedEntities[i]->sprite.verticies[2].position[1]) / 2;
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
		verticies.push_back(selectionBox.sprite.verticies[j]);
	}

	for (int i = 0; i < entities.size(); ++i)
	{
		if (collisionManager.AreSpritesCollided(selectionBox.sprite, entities[i]->sprite))
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

	for (ConnectionComponent* component : entities[i]->components)
	{
		if (collisionManager.IsSpriteClicked(component->sprite, inputHandler.mouseXPos, inputHandler.mouseYPos, inputHandler.currentInputEvent))
		{
			move = false;
			if (connectionComponent == NULL)
			{
				std::cout << "pressed";
				bool output = false;
				if (component->connectedTo != NULL && !component->outPutComponenet)
				{
					component->connectedTo->connectedTo = NULL;
					output = true;
				}
				component->connectedTo = NULL;

				if (!output)
				{

				connectionComponent = component;
				holdingComponent = true;
				}
			}

			else if (component->parentEntity != connectionComponent->parentEntity && (connectionComponent->outPutComponenet && !component->outPutComponenet || !connectionComponent->outPutComponenet && component->outPutComponenet))
			{
				if (component->connectedTo != NULL && component->connectedTo->outPutComponenet)
					component->connectedTo->connectedTo = NULL;

				component->connectedTo = connectionComponent;
				connectionComponent->connectedTo = component;
				std::cout << "added to" << (int)((LogicGate*)component->parentEntity)->m_type << std::endl;
				connectionComponent = NULL;				
			}
			else
			{
				connectionComponent = NULL;
			}
			break;
		}
	}

	if (entities[i]->clickable)
	{
		if (entities[i]->components[0] == connectionComponent)
		{
			connectionComponent = NULL;
			entities[i]->state = !entities[i]->state;
			holdingComponent = false;
		}
	}

	if (move)
		collisionManager.clickedEntity = entities[i];
	
}

void Program::AddSpritesToVertexBuffer()
{
	// Draws the line connecting a component and the mouse cursor
	if (connectionComponent != NULL)
	{
		float compenent1XCenter =
			(connectionComponent->sprite.verticies[0].position[0] + connectionComponent->sprite.verticies[1].position[0]) / 2;

		float compenent1YCenter =
			(connectionComponent->sprite.verticies[0].position[1] + connectionComponent->sprite.verticies[1].position[1]) / 2 + 0.05f;

		Sprite blackLine = spriteManager.CreateSprite(nameTextureLocationMapping["linePath"], compenent1XCenter, compenent1YCenter, inputHandler.mouseXPos, inputHandler.mouseYPos, 0.1f);

		for (int j = 0; j < 6; ++j)
			verticies.push_back(blackLine.verticies[j]);
	}

	// Draws the lines between components
	// This was done in a separate loop so that they are drawn beneath the components and entities
	for (int i = 0; i < entities.size(); ++i)
	{
		for (const ConnectionComponent* connectionComponent : entities[i]->components)
		{
			if (connectionComponent->connectedTo != NULL)
			{

				float compenent1XCenter =
					(connectionComponent->sprite.verticies[0].position[0] + connectionComponent->sprite.verticies[1].position[0]) / 2;

				float compenent1YCenter =
					(connectionComponent->sprite.verticies[0].position[1] + connectionComponent->sprite.verticies[1].position[1]) / 2 + 0.05f;

				float compenent2XCenter =
					(connectionComponent->connectedTo->sprite.verticies[0].position[0] + connectionComponent->connectedTo->sprite.verticies[1].position[0]) / 2;
				float compenent2YCenter =
					(connectionComponent->connectedTo->sprite.verticies[0].position[1] + connectionComponent->connectedTo->sprite.verticies[1].position[1]) / 2 + 0.05f;

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
			verticies.push_back(entities[i]->sprite.verticies[j]);

		for (const ConnectionComponent* connectionComponent : entities[i]->components)
		{
			for (int j = 0; j < 6; ++j)
				verticies.push_back(connectionComponent->sprite.verticies[j]);
		}
	}
}

void Program::AddHighlightBoxToEntity(Entity* entity)
{
	Entity selectionBox(spriteManager.CreateSprite(nameTextureLocationMapping["selectionBoxPath"], entity->sprite.m_xPos, entity->sprite.m_yPos, 1));
	for (int j = 0; j < 6; ++j)
	{
		verticies.push_back(selectionBox.sprite.verticies[j]);
	}
}

void Program::DeleteEntity(Entity* entity)
{
	for (auto component : entity->components)
	{
		if(component->connectedTo != NULL)
			(component->connectedTo)->connectedTo = NULL;
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

	gui.BeginNewFrame();
	inputHandler.m_inputReceived = false;
	spriteRenderer.PollEvents();
	inputHandler.UpdateInput();
	verticies.clear();
}

void Program::HandleCopyDeletePaste()
{
	if (inputHandler.currentInputEvent == InputEvents::COPYING)
			CopySelection();

	else if (inputHandler.currentInputEvent == InputEvents::PASTING && !copiedEntities.empty())
		PasteEntities();

	else if (inputHandler.currentInputEvent == InputEvents::DELETING)
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

	if (inputHandler.currentInputEvent == InputEvents::MOUSE_DRAG && selected != Selectable::NONE)
		AddMenuSelectedEntity();
	else
		selected = Selectable::NONE;
}

void Program::HandleMenuSelectedEntityPlacement()
{

	if (tempEntity != NULL && inputHandler.currentInputEvent == InputEvents::MOUSE_RELEASE && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AnyWindow))
	{
		delete tempEntity;
		tempEntity = NULL;

		entities.pop_back();
	}

	else if (tempEntity != NULL && inputHandler.currentInputEvent == InputEvents::MOUSE_RELEASE && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AnyWindow))
		tempEntity = NULL;
}

void Program::HandleEntitySelection()
{
	if (inputHandler.currentInputEvent == InputEvents::MOUSE_DRAG && tempEntity == NULL
		&& !selecting && collisionManager.clickedEntity == NULL && connectionComponent == NULL)
	{
		mouseStartXPos = inputHandler.mouseXPos;
		mouseStartYPos = inputHandler.mouseYPos;
		selecting = true;
	}
	else if (inputHandler.currentInputEvent == InputEvents::LEFT_MOUSE_CLICKED)
	{
		selecting = false;
		selectedEntities.clear();
	}


	if (inputHandler.currentInputEvent == InputEvents::MOUSE_RELEASE && selecting && !collisionManager.IsEntityBeingDragged(inputHandler.currentInputEvent))
		AddEntitiesToSelection();
	else if (selecting && !collisionManager.IsEntityBeingDragged(inputHandler.currentInputEvent))
	{
		mouseEndXPos = inputHandler.mouseXPos;
		mouseEndYPos = inputHandler.mouseYPos;

		Entity selectionBox(spriteManager.CreateSprite(nameTextureLocationMapping["selectionBoxPath"], mouseStartXPos, mouseStartYPos, mouseEndXPos, mouseEndYPos, 1, false));

		for (int j = 0; j < 6; ++j)
		{
			verticies.push_back(selectionBox.sprite.verticies[j]);
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
	if (collisionManager.IsEntityBeingDragged(inputHandler.currentInputEvent))
		collisionManager.clickedEntity->MoveToPoint(inputHandler.mouseXPos, inputHandler.mouseYPos);
	
	else
	{
		for (int i = 0; i < entities.size(); ++i)
		{
			if (collisionManager.IsSpriteClicked(entities[i]->sprite, inputHandler.mouseXPos, inputHandler.mouseYPos, inputHandler.currentInputEvent))
			{
				AttachClickedComponent(i);
				break;
			}
		}
	}

	if (connectionComponent != NULL && inputHandler.currentInputEvent == InputEvents::LEFT_MOUSE_CLICKED && !holdingComponent)
		connectionComponent = NULL;
}

void Program::UpdateEntitiesStates()
{
	for (int i = 0; i < entities.size(); ++i)
		entities[i]->Reset();
	for (int i = 0; i < entities.size(); ++i)
		entities[i]->Update();
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