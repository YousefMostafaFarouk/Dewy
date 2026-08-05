#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> 
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include "Program.h"
#include "Logicgate.h"
#include "Button.h"
#include "LightBulb.h"

#ifndef DEWY_SHADER_PATH
#define DEWY_SHADER_PATH "../Renderer/res/shaders/basic.shader"
#endif

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
	spriteRenderer(720, 1280, "LogicGateSimulator", DEWY_SHADER_PATH, spriteManager.m_textureSlots, spriteManager.m_numberOfTextures, proj),
	inputHandler(spriteRenderer.getWindowPointer()),
	gui(spriteRenderer)
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

Program::~Program()
{
	ClearCircuit();
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

void Program::HandleCircuitFileAction(const CircuitFileRequest& request)
{
	if (request.action == CircuitFileAction::NONE)
		return;

	try
	{
		const std::filesystem::path path(request.path);
		if (request.action == CircuitFileAction::SAVE)
		{
			CircuitSerializer::Save(path, BuildCircuitDocument());
			circuitFileStatus = "Saved circuit to " + path.string();
		}
		else
		{
			LoadCircuit(path);
		}
		circuitFileStatusIsError = false;
	}
	catch (const std::exception& error)
	{
		circuitFileStatus = error.what();
		circuitFileStatusIsError = true;
	}
}

void Program::LoadCircuit(const std::filesystem::path& path)
{
	LoadCircuitDocument(CircuitSerializer::Load(path));
	circuitFileStatus = "Loaded circuit from " + path.string();
	circuitFileStatusIsError = false;
}

CircuitDocument Program::BuildCircuitDocument() const
{
	CircuitDocument document;
	std::unordered_map<const Entity*, std::size_t> entityIds;

	for (std::size_t index = 0; index < entities.size(); ++index)
	{
		const Entity* entity = entities[index];
		entityIds.emplace(entity, index);

		CircuitEntityRecord record;
		record.id = index;
		record.x = entity->m_sprite.m_xPos;
		record.y = entity->m_sprite.m_yPos;
		record.state = entity->m_state;

		if (const auto* gate = dynamic_cast<const LogicGate*>(entity))
		{
			switch (gate->m_type)
			{
			case LogicGatesTypes::NOT: record.type = "not"; break;
			case LogicGatesTypes::OR: record.type = "or"; break;
			case LogicGatesTypes::AND: record.type = "and"; break;
			case LogicGatesTypes::XOR: record.type = "xor"; break;
			default: throw std::runtime_error("Cannot save an unknown logic gate type");
			}
		}
		else if (dynamic_cast<const Button*>(entity))
			record.type = "button";
		else if (dynamic_cast<const LightBulb*>(entity))
			record.type = "bulb";
		else
			throw std::runtime_error("Cannot save an unknown circuit entity type");

		document.entities.push_back(record);
	}

	for (std::size_t entityIndex = 0; entityIndex < entities.size(); ++entityIndex)
	{
		const Entity* entity = entities[entityIndex];
		for (std::size_t componentIndex = 0; componentIndex < entity->m_components.size(); ++componentIndex)
		{
			const ConnectionComponent* component = entity->m_components[componentIndex];
			if (!component->m_outPutComponenet || component->m_connectedTo == nullptr)
				continue;

			const ConnectionComponent* connected = component->m_connectedTo;
			const auto connectedEntity = entityIds.find(connected->m_parentEntity);
			if (connectedEntity == entityIds.end())
				throw std::runtime_error("Cannot save a connection to an entity outside the circuit");

			const auto& targetComponents = connected->m_parentEntity->m_components;
			const auto target = std::find(targetComponents.begin(), targetComponents.end(), connected);
			if (target == targetComponents.end())
				throw std::runtime_error("Cannot save a connection with an unknown target component");

			document.connections.push_back({
				entityIndex,
				componentIndex,
				connectedEntity->second,
				static_cast<std::size_t>(std::distance(targetComponents.begin(), target))
			});
		}
	}

	return document;
}

void Program::LoadCircuitDocument(const CircuitDocument& document)
{
	std::vector<Entity*> loadedEntities;
	std::unordered_map<std::size_t, Entity*> entitiesById;
	TopologicalOrder<Entity*> loadedOrder;

	try
	{
		for (const CircuitEntityRecord& record : document.entities)
		{
			Selectable type = Selectable::NONE;
			if (record.type == "not") type = Selectable::NOT;
			else if (record.type == "or") type = Selectable::OR;
			else if (record.type == "and") type = Selectable::AND;
			else if (record.type == "xor") type = Selectable::XOR;
			else if (record.type == "bulb") type = Selectable::BULB;
			else if (record.type == "button") type = Selectable::BUTTON;
			else throw std::runtime_error("Unsupported circuit entity type: " + record.type);

			Entity* entity = CreateEntity(type, record.x, record.y);
			entity->m_state = record.state;
			loadedEntities.push_back(entity);
			entitiesById.emplace(record.id, entity);
		}

		std::vector<std::pair<Entity*, Entity*>> graphEdges;
		graphEdges.reserve(document.connections.size());
		for (const CircuitConnectionRecord& record : document.connections)
			graphEdges.emplace_back(entitiesById.at(record.fromEntity), entitiesById.at(record.toEntity));

		if (!loadedOrder.Reset(loadedEntities, graphEdges))
			throw std::runtime_error("Circuit connections contain a cycle");

		for (const CircuitConnectionRecord& record : document.connections)
		{
			Entity* fromEntity = entitiesById.at(record.fromEntity);
			Entity* toEntity = entitiesById.at(record.toEntity);
			ConnectionComponent* output = fromEntity->m_components.at(record.fromComponent);
			ConnectionComponent* input = toEntity->m_components.at(record.toComponent);
			output->m_connectedTo = input;
			input->m_connectedTo = output;
		}
	}
	catch (...)
	{
		for (Entity* entity : loadedEntities)
			delete entity;
		throw;
	}

	ClearCircuit();
	entities = std::move(loadedEntities);
	circuitOrder = std::move(loadedOrder);
}

void Program::ClearCircuit()
{
	for (Entity* entity : entities)
		delete entity;
	entities.clear();

	for (Entity* entity : copiedEntities)
		delete entity;
	copiedEntities.clear();

	selectedEntities.clear();
	circuitOrder.Clear();
	tempEntity = nullptr;
	connectionComponent = nullptr;
	collisionManager.m_clickedEntity = nullptr;
	selected = Selectable::NONE;
}

void Program::AddMenuSelectedEntity()
{
	if (tempEntity == NULL)
	{
		tempEntity = CreateEntity();
		selected = Selectable::NONE;
		entities.push_back(tempEntity);
		circuitOrder.AddNode(tempEntity);
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
		circuitOrder.AddNode(copiedEntities[i]);
	}

	for (Entity* entity : copiedEntities)
	{
		for (ConnectionComponent* component : entity->m_components)
		{
			if (component->m_outPutComponenet && component->m_connectedTo != nullptr &&
				!circuitOrder.TryAddEdge(entity, component->m_connectedTo->m_parentEntity))
			{
				DisconnectComponent(component);
				circuitFileStatus = "A copied connection was removed because it would create a cycle";
				circuitFileStatusIsError = true;
			}
		}
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
				const bool disconnectedInput =
					component->m_connectedTo != NULL && !component->m_outPutComponenet;
				if (component->m_connectedTo != NULL)
					DisconnectComponent(component);

				if (!disconnectedInput)
				{
					connectionComponent = component;
					holdingComponent = true;
				}
			}

			else if (connectionComponent->m_outPutComponenet != component->m_outPutComponenet)
			{
				TryConnectComponents(connectionComponent, component);
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

void Program::DisconnectComponent(ConnectionComponent* component)
{
	if (component == nullptr || component->m_connectedTo == nullptr)
		return;

	ConnectionComponent* connected = component->m_connectedTo;
	ConnectionComponent* output = component->m_outPutComponenet ? component : connected;
	ConnectionComponent* input = component->m_outPutComponenet ? connected : component;

	if (output->m_outPutComponenet && !input->m_outPutComponenet)
	{
		bool anotherWireJoinsTheEntities = false;
		for (const ConnectionComponent* otherOutput : output->m_parentEntity->m_components)
		{
			if (otherOutput != output && otherOutput->m_outPutComponenet &&
				otherOutput->m_connectedTo != nullptr &&
				otherOutput->m_connectedTo->m_parentEntity == input->m_parentEntity)
			{
				anotherWireJoinsTheEntities = true;
				break;
			}
		}
		if (!anotherWireJoinsTheEntities)
			circuitOrder.RemoveEdge(output->m_parentEntity, input->m_parentEntity);
	}

	component->m_connectedTo = nullptr;
	connected->m_connectedTo = nullptr;
}

void Program::RestoreConnection(ConnectionComponent* output, ConnectionComponent* input)
{
	if (output == nullptr || input == nullptr)
		return;

	if (circuitOrder.TryAddEdge(output->m_parentEntity, input->m_parentEntity))
	{
		output->m_connectedTo = input;
		input->m_connectedTo = output;
	}
}

bool Program::TryConnectComponents(ConnectionComponent* first, ConnectionComponent* second)
{
	if (first == nullptr || second == nullptr ||
		first->m_outPutComponenet == second->m_outPutComponenet)
	{
		return false;
	}

	ConnectionComponent* output = first->m_outPutComponenet ? first : second;
	ConnectionComponent* input = first->m_outPutComponenet ? second : first;
	if (output->m_parentEntity == input->m_parentEntity)
	{
		circuitFileStatus = "Connection rejected: an entity cannot connect to itself";
		circuitFileStatusIsError = true;
		return false;
	}

	if (output->m_connectedTo == input && input->m_connectedTo == output)
		return true;

	ConnectionComponent* previousOutputTarget = output->m_connectedTo;
	ConnectionComponent* previousInputSource = input->m_connectedTo;
	if (previousOutputTarget != nullptr)
		DisconnectComponent(output);
	if (input->m_connectedTo != nullptr)
		DisconnectComponent(input);

	if (circuitOrder.TryAddEdge(output->m_parentEntity, input->m_parentEntity))
	{
		output->m_connectedTo = input;
		input->m_connectedTo = output;
		circuitFileStatus = "Connection added";
		circuitFileStatusIsError = false;
		return true;
	}

	RestoreConnection(output, previousOutputTarget);
	if (previousInputSource != output)
		RestoreConnection(previousInputSource, input);

	circuitFileStatus = "Connection rejected: it would create a cycle";
	circuitFileStatusIsError = true;
	return false;
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
			// Connections are stored symmetrically on both ports. Render only
			// from the output endpoint so each logical wire produces one line.
			if (connectionComponent->m_outPutComponenet && connectionComponent->m_connectedTo != NULL)
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
	circuitOrder.RemoveNode(entity);
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
	CircuitFileRequest circuitFileRequest;
	selected = gui.DrawMenu(
		spriteManager,
		nameTextureLocationMapping,
		circuitFileStatus,
		circuitFileStatusIsError,
		circuitFileRequest);
	HandleCircuitFileAction(circuitFileRequest);

	if (inputHandler.m_currentInputEvent == InputEvents::MOUSE_DRAG && selected != Selectable::NONE)
		AddMenuSelectedEntity();
	else
		selected = Selectable::NONE;
}

void Program::HandleMenuSelectedEntityPlacement()
{

	if (tempEntity != NULL && inputHandler.m_currentInputEvent == InputEvents::MOUSE_RELEASE && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AnyWindow))
	{
		circuitOrder.RemoveNode(tempEntity);
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
	for (Entity* entity : circuitOrder.Order())
		entity->Reset();
	for (Entity* entity : circuitOrder.Order())
		entity->Update();
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
	return CreateEntity(selected, inputHandler.mouseXPos, inputHandler.mouseYPos);
}

Entity* Program::CreateEntity(Selectable entityType, float xPosition, float yPosition)
{
	float buttonSize = 0.8f;
	Sprite smDot = spriteManager.CreateSprite(nameTextureLocationMapping["greenDotPath"], 2, 1, 0.090f);

	switch (entityType)
	{
	case Selectable::NOT:
		return new LogicGate(LogicGatesTypes::NOT, spriteManager.CreateSprite(nameTextureLocationMapping["notPath"], xPosition, yPosition, 1.0f), smDot);
	case Selectable::OR:
		return new LogicGate(LogicGatesTypes::OR, spriteManager.CreateSprite(nameTextureLocationMapping["orPath"], xPosition, yPosition, 1.0f), smDot);
	case Selectable::XOR:
		return new LogicGate(LogicGatesTypes::XOR, spriteManager.CreateSprite(nameTextureLocationMapping["xorPath"], xPosition, yPosition, 1.0f), smDot);
	case Selectable::AND:
		return new LogicGate(LogicGatesTypes::AND, spriteManager.CreateSprite(nameTextureLocationMapping["andPath"], xPosition, yPosition, 1.0f), smDot);
	case Selectable::BULB:
		return new LightBulb(spriteManager.CreateSprite(nameTextureLocationMapping["offLightBulbPath"], xPosition, yPosition, 1), smDot,
			spriteManager.GetTextureSlot(nameTextureLocationMapping["onLightBulbPath"]), spriteManager.GetTextureSlot(nameTextureLocationMapping["offLightBulbPath"]));
	case Selectable::BUTTON:
		return new Button(spriteManager.CreateSprite(nameTextureLocationMapping["unPressedButtonPath"], xPosition, yPosition, 1.0), smDot,
			spriteManager.CreateSprite(nameTextureLocationMapping["clearPath"], 0.0f, 0.0f, 0.25f * buttonSize), spriteManager.GetTextureSlot(nameTextureLocationMapping["pressedButtonPath"]), spriteManager.GetTextureSlot(nameTextureLocationMapping["unPressedButtonPath"]));
	default:
		return NULL;
	}
}
