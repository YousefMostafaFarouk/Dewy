#include "Button.h"
#include "ConnectionComponent.h"

Button::Button(const Sprite& buttonSprite, const Sprite& componentSprite, const Sprite& transparentComponentSprite, int onTextureId, int offTextureId) : Entity(buttonSprite), m_offTextureId(offTextureId), m_onTextureId(onTextureId)
{
	m_clickable = true;
	// The first component is pretty much the hitbox for the button
	m_components.push_back(new ConnectionComponent(transparentComponentSprite, this, 0.5 * m_sprite.m_size, -0.5f * m_sprite.m_size, true));

	m_components.push_back(new ConnectionComponent(componentSprite, this, 1.0f * m_sprite.m_size, -0.5f * m_sprite.m_size, true));
}

Entity* Button::Copy()
{
	Entity* copiedButton = new Button(m_sprite, m_components[1]->m_sprite, m_components[0]->m_sprite, m_onTextureId, m_offTextureId);
	copiedButton->m_state = m_state;
	return copiedButton;
}


void Button::MoveToPoint(float xPos, float yPos)
{
	m_sprite.MoveToPoint(xPos, yPos);

	for (auto& component : m_components)
	{
		component->MoveToPoint(0, 0);
	}
}

void Button::MoveAlongVector(float xPos, float yPos)
{
	m_sprite.MoveAlongVector(xPos, yPos);

	for (auto& component : m_components)
	{
		component->MoveToPoint(0, 0);
	}
}

bool Button::Update()
{
	if (m_updated)
		return m_state;

	if (m_state == 0)
		m_sprite.UpdateTextureID(m_offTextureId);
	else
		m_sprite.UpdateTextureID(m_onTextureId);

	m_updated = true;
	return m_state;
}
