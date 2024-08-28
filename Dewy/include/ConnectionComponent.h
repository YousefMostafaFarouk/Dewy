#pragma once
#include "Entity.h"
class ConnectionComponent
{
public:
	Sprite sprite;
	ConnectionComponent* connectedTo{ NULL };
	Entity* parentEntity{ NULL };

	float xOffSet;
	float yOffSet;

	bool outPutComponenet{ false };

public:
	ConnectionComponent(const Sprite& componentSprite, Entity* parent, float xOffSet, float yOffset, bool outputComp=false);
	void MoveToPoint(float xPos, float yPos);
};

