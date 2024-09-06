#pragma once
#include "Sprite.h"
#include "iostream"
class ConnectionComponent;

class Entity
{
public:
	Entity(const Sprite& m_sprite) : m_sprite(m_sprite) {};
	virtual bool Update() { return false; };
	virtual Entity* OnClick(Entity* entity, float xPos, float yPos) { return NULL; };
	virtual void MoveToPoint(float xPos, float yPos) { m_sprite.MoveToPoint(xPos, yPos); };
	virtual void MoveAlongVector(float xPos, float yPos) { m_sprite.MoveAlongVector(xPos, yPos); };
	virtual ~Entity() { for (auto& component : m_components) delete component; std::cout << "deleted\n"; };
	virtual void Reset() { m_updated = false; };
	virtual Entity* Copy() { return nullptr; };
public:
	bool m_state;
	std::vector<ConnectionComponent*> m_components;
	bool m_updated{ false };
	bool m_clickable{ false };
	Sprite m_sprite;

};



