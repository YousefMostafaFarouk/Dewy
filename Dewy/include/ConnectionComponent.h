#pragma once
#include "Entity.h"
class ConnectionComponent
{
public:
	Sprite m_sprite;
	ConnectionComponent* m_connectedTo{ NULL };
	Entity* m_parentEntity{ NULL };

	float m_xOffSet;
	float m_yOffSet;

	bool m_outPutComponenet{ false };

public:
	ConnectionComponent(const Sprite& componentSprite, Entity* parent, float m_xOffSet, float yOffset, bool outputComp=false);
	void MoveToPoint(float xPos, float yPos);
};

