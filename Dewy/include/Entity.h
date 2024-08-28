#pragma once
#include "Sprite.h"
#include "iostream"
class ConnectionComponent;

class Entity
{
public:
	Entity(const Sprite& sprite) : sprite(sprite) {};
	virtual bool Update() { return false; };
	virtual Entity* OnClick(Entity* entity, float xPos, float yPos) { return NULL; };
	virtual void MoveToPoint(float xPos, float yPos) { sprite.MoveToPoint(xPos, yPos); };
	virtual void MoveAlongVector(float xPos, float yPos) { sprite.MoveAlongVector(xPos, yPos); };
	virtual ~Entity() { for (auto& component : components) delete component; std::cout << "deleted\n"; };
	virtual void Reset() { updated = false; };
	virtual Entity* Copy() { return nullptr; };
public:
	bool state;
	std::vector<ConnectionComponent*> components;
	bool updated{ false };
	bool clickable{ false };
	Sprite sprite;

};



