#pragma once
#include "Entity.h"

class LightBulb : public Entity
{
private:
	int m_onTextureId;
	int m_offTextureId;

public:
	LightBulb(const Sprite& lightBulbSprite, const Sprite& componentSprite, int onTextureId, int offTextureId);
	virtual Entity* Copy() override;
	virtual void MoveToPoint(float xPos, float yPos) override;
	virtual void MoveAlongVector(float xPos, float yPos) override;
	virtual bool Update() override;
};

