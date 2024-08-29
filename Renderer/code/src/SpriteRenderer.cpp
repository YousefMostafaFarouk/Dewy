#include "SpriteRenderer.h"
#include "iostream"

SpriteRender::SpriteRender(unsigned int height, unsigned int width, const std::string& name, const std::string& shaderPath,
const int* textureLookUp, const int numberOfTextures, glm::mat4 projectionMatrix)
: Renderer(height,width,name), m_shader(shaderPath), m_textureIndexLookUp(textureLookUp), m_numberOfTextures(numberOfTextures), m_projectionMatrix(projectionMatrix)
{
	spriteBufferLayout.Push<float>(2);
	spriteBufferLayout.Push<float>(2);
	spriteBufferLayout.Push<float>(1);

	spriteVertexArray.AddBuffer(spriteVertexBuffer, spriteBufferLayout);

	m_shader.Bind();
	m_shader.SetUniform1i("u_NumberOfTextures", m_numberOfTextures);
	m_shader.SetUniform1iv("u_Texture", m_numberOfTextures, m_textureIndexLookUp);
	m_shader.SetUniformMat4f("u_MVP", m_projectionMatrix);

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		});
}

void SpriteRender::DrawSprite(const std::vector<Vertex>& verticies)
{
	spriteVertexArray.Bind();
	spriteVertexBuffer.AddBufferData(verticies.size() * sizeof(Vertex), &verticies[0], verticies.size());
	GLCall(glDrawArrays(GL_TRIANGLES, 0, spriteVertexBuffer.GetVerticiesCount()));
}

void SpriteRender::UpdateProjectionMatrix(glm::mat4 projectionMatrix)
{
	m_projectionMatrix = projectionMatrix;
	m_shader.SetUniformMat4f("u_MVP", m_projectionMatrix);
}


