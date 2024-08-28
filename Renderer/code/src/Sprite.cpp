#include <algorithm>
#include <iostream>
#include "Sprite.h"

Sprite::Sprite(float xPos, float yPos, float size, float textureID) :m_texID(textureID), m_size(size), m_xPos(xPos), m_yPos(yPos)
{
	for (int i = 0; i < 6; ++i)
	{

		verticies[i].position[0] *= size;
		verticies[i].position[1] *= size;

		verticies[i].position[0] += xPos;
		verticies[i].position[1] += yPos;

		verticies[i].texID = textureID;
	}
}

void Sprite::MakeTriangle(float bottomLeftXPos, float bottomLeftYPos,
	float bottomRightXPos, float bottomRightYPos,
	float topRightXPos, float topRightYPos,
	float topLeftXPos, float topLeftYPos) {

	verticies[0].position[0] = bottomLeftXPos;
	verticies[0].position[1] = bottomLeftYPos;

	verticies[1].position[0] = bottomRightXPos;
	verticies[1].position[1] = bottomRightYPos;

	verticies[2].position[0] = topRightXPos;
	verticies[2].position[1] = topRightYPos;

	verticies[3].position[0] = topRightXPos;
	verticies[3].position[1] = topRightYPos;

	verticies[4].position[0] = topLeftXPos;
	verticies[4].position[1] = topLeftYPos;

	verticies[5].position[0] = bottomLeftXPos;
	verticies[5].position[1] = bottomLeftYPos;
}


Sprite::Sprite(float x1Pos, float y1Pos, float x2Pos, float y2Pos, float size, float textureID, bool centered) :m_texID(textureID), m_size(size), m_xPos(x2Pos-x1Pos), m_yPos(y2Pos-y1Pos)
{
	if (centered) 
	{
		float dx = x2Pos - x1Pos;
		float dy = y2Pos - y1Pos;

		// Calculate the perpendicular vector
		float perpX = -dy;
		float perpY = dx;

		// Normalize the perpendicular vector
		float length = sqrt(perpX * perpX + perpY * perpY);
		perpX /= length;
		perpY /= length;

		// Calculate the points for the rectangle
		float halfWidth = size / 2;
		float halfHeight = size / 2;

		float topLeftX = x1Pos + perpX * halfWidth;
		float topLeftY = y1Pos + perpY * halfWidth;

		float bottomLeftX = x1Pos - perpX * halfWidth;
		float bottomLeftY = y1Pos - perpY * halfWidth;

		float topRightX = x2Pos + perpX * halfWidth;
		float topRightY = y2Pos + perpY * halfWidth;

		float bottomRightX = x2Pos - perpX * halfWidth;
		float bottomRightY = y2Pos - perpY * halfWidth;

		MakeTriangle(bottomLeftX, bottomLeftY, bottomRightX, bottomRightY, topRightX, topRightY, topLeftX, topLeftY);

		for (int i = 0; i < 6; ++i)
			verticies[i].texID = textureID;
	}
	else
	{

		float topLeftX = x1Pos ;
		float topLeftY = y1Pos ;

		float bottomLeftX = x1Pos ;
		float bottomLeftY = y2Pos ;

		float topRightX = x2Pos ;
		float topRightY = y1Pos ;

		float bottomRightX = x2Pos ;
		float bottomRightY = y2Pos ;

		// Define the two triangles to form the rectangle
		MakeTriangle(bottomLeftX, bottomLeftY, bottomRightX, bottomRightY, topRightX, topRightY, topLeftX, topLeftY);

		for (int i = 0; i < 6; ++i)
			verticies[i].texID = textureID;
	}
}

void Sprite::MoveToPoint(float xPos, float yPos)
{
	float centerX = (verticies[0].position[0] + verticies[2].position[0]) / 2;
	float centerY = (verticies[0].position[1] + verticies[2].position[1]) / 2;

	float shiftX = xPos - centerX;
	float shiftY = yPos - centerY;

	m_xPos += shiftX;
	m_yPos += shiftY;

	for (int i = 0; i < 6; ++i)
	{
		verticies[i].position[0] += shiftX;
		verticies[i].position[1] += shiftY;
	}
}

void Sprite::MoveAlongVector(float xPos, float yPos)
{
	m_xPos += xPos;
	m_yPos += yPos;

	for (int i = 0; i < 6; ++i)
	{
		verticies[i].position[0] += xPos;
		verticies[i].position[1] += yPos;
	}
}

void Sprite::UpdateTextureID(float ID)
{
	m_texID = ID;

	for (int i = 0; i < 6; ++i)
		verticies[i].texID = m_texID;
}

