#include "logicgate.h"
#include <iostream>

LogicGate::LogicGate(LogicGatesTypes type, const Sprite& sprite, const Sprite& componentSprite)
:Entity(sprite), m_type(type)
{
	switch (type)
	{
	case LogicGatesTypes::NONE:
		break;

	case LogicGatesTypes::NOT:
		components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * sprite.m_size, -0.5f * sprite.m_size));
		components.push_back(new ConnectionComponent(componentSprite, this, 0.9f * sprite.m_size, -0.5f * sprite.m_size, true));
		break;

	case LogicGatesTypes::OR:
		components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * sprite.m_size, -0.30f * sprite.m_size));
		components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * sprite.m_size, -0.70f * sprite.m_size));
		components.push_back(new ConnectionComponent(componentSprite, this, 0.9f * sprite.m_size, -0.5f * sprite.m_size, true));
		break;

	case LogicGatesTypes::AND:
		components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * sprite.m_size, -0.30f * sprite.m_size));
		components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * sprite.m_size, -0.70f * sprite.m_size));
		components.push_back(new ConnectionComponent(componentSprite, this, 0.9f * sprite.m_size, -0.5f * sprite.m_size, true));
		break;

	case LogicGatesTypes::XOR:
		components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * sprite.m_size, -0.30f * sprite.m_size));
		components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * sprite.m_size, -0.70f * sprite.m_size));
		components.push_back(new ConnectionComponent(componentSprite, this, 0.9f * sprite.m_size, -0.5f * sprite.m_size, true));
		break;

	default:
		break;
	}
}

LogicGate::LogicGate(LogicGatesTypes type, const Sprite& sprite)
	:Entity(sprite), m_type(type)
{}

Entity* LogicGate::OnClick(Entity * entity, float xPos, float yPos)
{
	return NULL;
}

void LogicGate::MoveToPoint(float xPos, float yPos)
{
	sprite.MoveToPoint(xPos, yPos);
	
	for (auto& component : components)
	{
		component->MoveToPoint(0, 0);
	}
}

void LogicGate::MoveAlongVector(float xPos, float yPos)
{
	sprite.MoveAlongVector(xPos, yPos);

	for (auto& component : components)
	{
		component->MoveToPoint(0, 0);
	}
}

bool LogicGate::Update()
{
	if (updated)
		return state;

	bool result = false;

	switch (m_type)
	{
	case LogicGatesTypes::AND:
		result =AndUpdate();
		break;

	case LogicGatesTypes::OR:
		result =OrUpdate();
		break;

	case LogicGatesTypes::NOT:
		result =NotUpdate();
		break;
	case LogicGatesTypes::XOR:
		result =XorUpdate();
	}

	updated = true;
	return result;
}

Entity* LogicGate::Copy()
{
	Entity* copiedLogicGate = new LogicGate(m_type, sprite, components[0]->sprite);
	return copiedLogicGate;
}

bool LogicGate::AndUpdate()
{
	bool result = true;
	for (const ConnectionComponent* component : components)
	{
		if (component->outPutComponenet)
			continue;

		if (component->connectedTo == NULL)
			result &= false;
		else
			result &= component->connectedTo->parentEntity->Update();
	}
	state = result;
	return state;
}

bool LogicGate::OrUpdate()
{
	bool result = false;
	for (const ConnectionComponent* component : components) 
	{
		if (component->outPutComponenet)
			continue;

		if (component->connectedTo == NULL)
			result |= false;

		else
			result |= component->connectedTo->parentEntity->Update();
	}
	state = result;
	//std::cout << result << std::endl;
	return state;
}

bool LogicGate::XorUpdate()
{

	bool result = false;
	for (const ConnectionComponent* component : components)
	{
		if (component->outPutComponenet)
			continue;

		if (component->connectedTo == NULL)
			result ^= false;
		else
			result ^= component->connectedTo->parentEntity->Update();
	}
	state = result;
	return state;
}

bool LogicGate::NotUpdate()
{
	for (const ConnectionComponent* component : components)
	{
		if (component->outPutComponenet)
			continue;
		if (component->connectedTo == NULL)
			state = !false;
		else
			state = !component->connectedTo->parentEntity->Update();
	}
	return state;
}