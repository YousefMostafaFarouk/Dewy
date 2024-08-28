#include "ConnectionComponent.h"
ConnectionComponent::ConnectionComponent(const Sprite& componentSprite, Entity* parent, float xOffset, float yOffset, bool outputComp) :sprite(componentSprite), parentEntity(parent), xOffSet(xOffset), yOffSet(yOffset), outPutComponenet(outputComp)
{
	MoveToPoint(0, 0);
}

void ConnectionComponent::MoveToPoint(float xAddedToOffset =0, float yAddedToOffset=0)
{
	sprite.MoveToPoint(parentEntity->sprite.verticies[4].position[0] + xOffSet + xAddedToOffset, parentEntity->sprite.verticies[4].position[1] + yOffSet + yAddedToOffset);
}