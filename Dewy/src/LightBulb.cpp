#include "LightBulb.h"
#include "ConnectionComponent.h"

LightBulb::LightBulb(const Sprite& lightBulbSprite, const Sprite& componentSprite, int onTextureId, int offTextureId) : Entity(lightBulbSprite), m_offTextureId(offTextureId), m_onTextureId(onTextureId)
{
	m_components.push_back(new ConnectionComponent(componentSprite, this, 1.0f * m_sprite.m_size, -0.5f * m_sprite.m_size, false));
}

Entity* LightBulb::Copy()
{
	Entity* copiedLightBulb = new LightBulb(m_sprite, m_components[0]->m_sprite, m_onTextureId, m_offTextureId);
	return copiedLightBulb;
}

void LightBulb::MoveToPoint(float xPos, float yPos)
{
	m_sprite.MoveToPoint(xPos, yPos);

	for (auto& component : m_components)
	{
		component->MoveToPoint(0, 0);
	}
}

void LightBulb::MoveAlongVector(float xPos, float yPos)
{
	m_sprite.MoveAlongVector(xPos, yPos);

	for (auto& component : m_components)
	{
		component->MoveToPoint(0, 0);
	}
}

bool LightBulb::Update()
{
	if (m_updated)
		return m_state;

	if (m_components[0]->m_connectedTo != NULL && m_components[0]->m_connectedTo->m_parentEntity->m_state)
		m_sprite.UpdateTextureID(m_onTextureId);
	else
		m_sprite.UpdateTextureID(m_offTextureId);

	m_updated = true;
	return m_state;
}