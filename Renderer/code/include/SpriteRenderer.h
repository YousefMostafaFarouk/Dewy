#pragma once
#include"Sprite.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "vector"
#include "Shader.h"


class SpriteRender : public Renderer
{
private:
	VertexBufferLayout spriteBufferLayout;
	VertexArray spriteVertexArray;
	VertexBuffer spriteVertexBuffer;
	Shader m_shader;

	const int* m_textureIndexLookUp;
	const int m_numberOfTextures;
	glm::mat4 m_projectionMatrix;

public:
	SpriteRender(unsigned int height, unsigned int width, const std::string& name, const std::string& shaderPath
	, const int* textureLookUp, const int numberOfTextures, glm::mat4 projectionMatrix);
	void DrawSprite(const std::vector<Vertex>& verticies);
	void UpdateProjectionMatrix(glm::mat4 projectionMatrix);
};