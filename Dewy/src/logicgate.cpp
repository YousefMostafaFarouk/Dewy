#include "logicgate.h"
#include <iostream>

LogicGate::LogicGate(LogicGatesTypes type, const Sprite& m_sprite, const Sprite& componentSprite)
:Entity(m_sprite), m_type(type)
{
	switch (type)
	{
	case LogicGatesTypes::NONE:
		break;

	case LogicGatesTypes::NOT:
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * m_sprite.m_size, -0.5f * m_sprite.m_size));
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.9f * m_sprite.m_size, -0.5f * m_sprite.m_size, true));
		break;

	case LogicGatesTypes::OR:
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * m_sprite.m_size, -0.30f * m_sprite.m_size));
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * m_sprite.m_size, -0.70f * m_sprite.m_size));
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.9f * m_sprite.m_size, -0.5f * m_sprite.m_size, true));
		break;

	case LogicGatesTypes::AND:
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * m_sprite.m_size, -0.30f * m_sprite.m_size));
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * m_sprite.m_size, -0.70f * m_sprite.m_size));
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.9f * m_sprite.m_size, -0.5f * m_sprite.m_size, true));
		break;

	case LogicGatesTypes::XOR:
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * m_sprite.m_size, -0.30f * m_sprite.m_size));
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.125f * m_sprite.m_size, -0.70f * m_sprite.m_size));
		m_components.push_back(new ConnectionComponent(componentSprite, this, 0.9f * m_sprite.m_size, -0.5f * m_sprite.m_size, true));
		break;

	default:
		break;
	}
}

LogicGate::LogicGate(LogicGatesTypes type, const Sprite& m_sprite)
	:Entity(m_sprite), m_type(type)
{}

Entity* LogicGate::OnClick(Entity * entity, float xPos, float yPos)
{
	return NULL;
}

void LogicGate::MoveToPoint(float xPos, float yPos)
{
	m_sprite.MoveToPoint(xPos, yPos);
	
	for (auto& component : m_components)
	{
		component->MoveToPoint(0, 0);
	}
}

void LogicGate::MoveAlongVector(float xPos, float yPos)
{
	m_sprite.MoveAlongVector(xPos, yPos);

	for (auto& component : m_components)
	{
		component->MoveToPoint(0, 0);
	}
}

bool LogicGate::Update()
{
	if (m_updated)
		return m_state;

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

	m_updated = true;
	return result;
}

Entity* LogicGate::Copy()
{
	Entity* copiedLogicGate = new LogicGate(m_type, m_sprite, m_components[0]->m_sprite);
	return copiedLogicGate;
}

bool LogicGate::AndUpdate()
{
	bool result = true;
	for (const ConnectionComponent* component : m_components)
	{
		if (component->m_outPutComponenet)
			continue;

		if (component->m_connectedTo == NULL)
			result &= false;
		else
			result &= component->m_connectedTo->m_parentEntity->Update();
	}
	m_state = result;
	return m_state;
}

bool LogicGate::OrUpdate()
{
	bool result = false;
	for (const ConnectionComponent* component : m_components) 
	{
		if (component->m_outPutComponenet)
			continue;

		if (component->m_connectedTo == NULL)
			result |= false;

		else
			result |= component->m_connectedTo->m_parentEntity->Update();
	}
	m_state = result;
	//std::cout << result << std::endl;
	return m_state;
}

bool LogicGate::XorUpdate()
{

	bool result = false;
	for (const ConnectionComponent* component : m_components)
	{
		if (component->m_outPutComponenet)
			continue;

		if (component->m_connectedTo == NULL)
			result ^= false;
		else
			result ^= component->m_connectedTo->m_parentEntity->Update();
	}
	m_state = result;
	return m_state;
}

bool LogicGate::NotUpdate()
{
	for (const ConnectionComponent* component : m_components)
	{
		if (component->m_outPutComponenet)
			continue;
		if (component->m_connectedTo == NULL)
			m_state = !false;
		else
			m_state = !component->m_connectedTo->m_parentEntity->Update();
	}
	return m_state;
}