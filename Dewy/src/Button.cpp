#include "Button.h"
#include "ConnectionComponent.h"

Button::Button(const Sprite& buttonSprite, const Sprite& componentSprite, const Sprite& transparentComponentSprite, int onTextureId, int offTextureId) : Entity(buttonSprite), m_offTextureId(offTextureId), m_onTextureId(onTextureId)
{
	clickable = true;
	// The first component is pretty much the hitbox for the button
	components.push_back(new ConnectionComponent(transparentComponentSprite, this, 0.5 * sprite.m_size, -0.5f * sprite.m_size, true));

	components.push_back(new ConnectionComponent(componentSprite, this, 1.0f * sprite.m_size, -0.5f * sprite.m_size, true));
}

Entity* Button::Copy()
{
	Entity* copiedButton = new Button(sprite, components[1]->sprite, components[0]->sprite, m_onTextureId, m_offTextureId);
	copiedButton->state = state;
	return copiedButton;
}


void Button::MoveToPoint(float xPos, float yPos)
{
	sprite.MoveToPoint(xPos, yPos);

	for (auto& component : components)
	{
		component->MoveToPoint(0, 0);
	}
}

void Button::MoveAlongVector(float xPos, float yPos)
{
	sprite.MoveAlongVector(xPos, yPos);

	for (auto& component : components)
	{
		component->MoveToPoint(0, 0);
	}
}

bool Button::Update()
{
	if (updated)
		return state;

	if (state == 0)
		sprite.UpdateTextureID(m_offTextureId);
	else
		sprite.UpdateTextureID(m_onTextureId);

	updated = true;
	return state;
}
