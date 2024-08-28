#pragma once
#include "Texture.h"

struct Vertex
{
	float position[2];
	float texCoords[2];
	float texID;
};

class Sprite
{
public:
	Vertex verticies[6] = 
	{
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
		0.5f,  0.5f, 1.0f, 1.0f, 0.0f,
	   -0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	   -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	};
	float m_xPos, m_yPos;
	float m_size;

public:

	Sprite(float xPos, float yPos, float size, float textureID);
	Sprite(float x1Pos, float y1Pos, float x2Pos, float y2Pos, float size, float textureID, bool centered=true);
	Sprite() {};

	void MoveToPoint(float xPos, float yPos);

	void MoveAlongVector(float xPos, float yPos);

	void UpdateTextureID(float ID);

	void MakeTriangle(float bottomLeftXPos, float bottomLeftYPos, float bottomRightXPos, float bottomRightYPos,
		float topRightXPos, float topRightYPos,
		float topLeftXPos, float topLeftYPos);
private:
	float m_texID;
};