#include <algorithm>
#include "CollisionManager.h"
#include <iostream>
bool CollisionManager::AABBCollision(float firstLeftXPos, float firstRightXPos, float firstDownYPos, float firstUpYPos
, float secondLeftXPos, float secondRightXPos, float secondDownYPos, float secondUpYPos)
{
	bool collidedHorizontally = firstRightXPos >= secondLeftXPos && secondRightXPos >= firstLeftXPos;

	bool collidedVertically = firstUpYPos >= secondDownYPos && secondUpYPos >= firstDownYPos;

	return collidedHorizontally && collidedVertically;
}

bool CollisionManager::IsSpriteClicked(Sprite& sprite, float mouseXPos, float mouseYPos, InputEvents currentInputEvent)
{
	if(currentInputEvent != InputEvents::LEFT_MOUSE_CLICKED)
		return false;

	float mouseWidth = 0.1f;
	float mouseHeight = 0.1f;

	bool result = AABBCollision(mouseXPos - mouseWidth, mouseXPos + mouseWidth, mouseYPos - mouseHeight, mouseYPos + mouseHeight,
	sprite.verticies[0].position[0], sprite.verticies[2].position[0],
	sprite.verticies[0].position[1], sprite.verticies[2].position[1]);

	return result;
}

bool CollisionManager::IsCollidedWithAnyEntity(Entity* entity, std::vector<Entity*>& allEntities)
{
	return false;
}

bool CollisionManager::AreSpritesCollided(Sprite& sprite1, Sprite& sprite2)
{
	const float MIN_FLOAT = -100000;
	const float MAX_FLOAT = 100000;

	float firstLeftXPos = MAX_FLOAT;
	float firstRightXPos = MIN_FLOAT;
	float firstDownYPos = MAX_FLOAT;
	float firstUpYPos   = MIN_FLOAT;

	float secondLeftXPos = MAX_FLOAT;
	float secondRightXPos = MIN_FLOAT;
	float secondDownYPos = MAX_FLOAT;
	float secondUpYPos = MIN_FLOAT;

	for (int i = 0; i < 6; ++i)
	{
		firstLeftXPos = std::min(sprite1.verticies[i].position[0], firstLeftXPos);
		firstRightXPos = std::max(sprite1.verticies[i].position[0], firstRightXPos);
		firstDownYPos = std::min(sprite1.verticies[i].position[1], firstDownYPos);
		firstUpYPos = std::max(sprite1.verticies[i].position[1], firstUpYPos);

		secondLeftXPos = std::min(sprite2.verticies[i].position[0], secondLeftXPos);
		secondRightXPos = std::max(sprite2.verticies[i].position[0], secondRightXPos);
		secondDownYPos = std::min(sprite2.verticies[i].position[1], secondDownYPos);
		secondUpYPos = std::max(sprite2.verticies[i].position[1], secondUpYPos);
	}


	bool result = AABBCollision(firstLeftXPos, firstRightXPos, firstDownYPos, firstUpYPos, secondLeftXPos, secondRightXPos, secondDownYPos, secondUpYPos);
	return result;
}

bool CollisionManager::IsEntityBeingDragged(InputEvents currentInputEvent)
{
	if ((currentInputEvent == InputEvents::MOUSE_DRAG ) && clickedEntity != NULL)
		return true;

	if(currentInputEvent == InputEvents::MOUSE_RELEASE)
		clickedEntity = NULL;

	return false;
}
