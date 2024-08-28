#pragma once
#include "Entity.h"

class Button : public Entity
{
private:
	int m_onTextureId;
	int m_offTextureId;
public:
	Button(const Sprite& buttonSprite, const Sprite& componentSprite, const Sprite& transparentComponentSprite, int onTextureId, int offTextureId);
	virtual Entity* Copy();
	virtual void MoveToPoint(float xPos, float yPos) override;
	virtual void MoveAlongVector(float xPos, float yPos) override;
	virtual bool Update() override;
};

