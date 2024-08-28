#include "LightBulb.h"
#include "ConnectionComponent.h"

LightBulb::LightBulb(const Sprite& lightBulbSprite, const Sprite& componentSprite, int onTextureId, int offTextureId) : Entity(lightBulbSprite), m_offTextureId(offTextureId), m_onTextureId(onTextureId)
{
	components.push_back(new ConnectionComponent(componentSprite, this, 1.0f * sprite.m_size, -0.5f * sprite.m_size, false));
}

Entity* LightBulb::Copy()
{
	Entity* copiedLightBulb = new LightBulb(sprite, components[0]->sprite, m_onTextureId, m_offTextureId);
	return copiedLightBulb;
}

void LightBulb::MoveToPoint(float xPos, float yPos)
{
	sprite.MoveToPoint(xPos, yPos);

	for (auto& component : components)
	{
		component->MoveToPoint(0, 0);
	}
}

void LightBulb::MoveAlongVector(float xPos, float yPos)
{
	sprite.MoveAlongVector(xPos, yPos);

	for (auto& component : components)
	{
		component->MoveToPoint(0, 0);
	}
}

bool LightBulb::Update()
{
	if (updated)
		return state;

	if (components[0]->connectedTo != NULL && components[0]->connectedTo->parentEntity->state)
		sprite.UpdateTextureID(m_onTextureId);
	else
		sprite.UpdateTextureID(m_offTextureId);

	updated = true;
	return state;
}