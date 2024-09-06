#include "ConnectionComponent.h"
ConnectionComponent::ConnectionComponent(const Sprite& componentSprite, Entity* parent, float xOffset, float yOffset, bool outputComp) :m_sprite(componentSprite), m_parentEntity(parent), m_xOffSet(xOffset), m_yOffSet(yOffset), m_outPutComponenet(outputComp)
{
	MoveToPoint(0, 0);
}

void ConnectionComponent::MoveToPoint(float xAddedToOffset =0, float yAddedToOffset=0)
{
	m_sprite.MoveToPoint(m_parentEntity->m_sprite.verticies[4].position[0] + m_xOffSet + xAddedToOffset, m_parentEntity->m_sprite.verticies[4].position[1] + m_yOffSet + yAddedToOffset);
}