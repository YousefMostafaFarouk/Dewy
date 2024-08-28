#pragma once
#include <vector>
#include "entity.h"
#include "Sprite.h"
#include "ConnectionComponent.h"

enum class LogicGatesTypes { NONE, NOT, OR, AND, XOR };

class LogicGate : public Entity
{
public:
	LogicGatesTypes m_type;

public:
	LogicGate(LogicGatesTypes type, const Sprite& sprite, const Sprite& componentSprite);
	LogicGate(LogicGatesTypes type, const Sprite& sprite);
	virtual Entity* OnClick(Entity* entity, float xPos, float yPos) override;
	virtual void MoveToPoint(float xPos, float yPos) override;
	virtual void MoveAlongVector(float xPos, float yPos) override;
	virtual bool Update() override;
	virtual Entity* Copy() override;
	
private:
	std::vector<Entity*> m_connectedEntities;

private:

	bool AndUpdate();

	bool OrUpdate();

	bool XorUpdate();

	bool NotUpdate();
};