#pragma once
#include <vector>
#include "Entity.h"
#include "InputHandler.h"

class CollisionManager
{
private:
	bool AABBCollision(float firstLeftXPos, float firstRightXPos, float firstUpYPos, float firstDownYPos, float secondLeftXPos, 
		float secondRightXPos, float secondUpYPos, float secondDownYPos);
public:
	bool IsSpriteClicked(Sprite& m_sprite, float mouseXPos, float mouseYPos, InputEvents currentInputEvent);
	bool IsCollidedWithAnyEntity(Entity* entity, std::vector<Entity*> &entities);
	bool AreSpritesCollided(Sprite& sprite1, Sprite& sprite2);
	bool IsEntityBeingDragged(InputEvents currentInputEvent);
public:
	Entity* m_clickedEntity = NULL;

};

