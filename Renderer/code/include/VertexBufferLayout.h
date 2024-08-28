#pragma once
#include <vector>
#include <GL/glew.h>
#include "Renderer.h"

struct VertexBufferElement
{
	unsigned int count;
	unsigned int type;
	unsigned char normalized;

	VertexBufferElement(unsigned int t, unsigned int c, bool n) :
		type(t), count(c), normalized(n)
	{}

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:         return 4;
		case GL_UNSIGNED_BYTE: return 1;
		case GL_UNSIGNED_INT:  return 4;
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_elements;
	unsigned int m_stride;
public:
	VertexBufferLayout() :m_stride(0) {};

	template<typename T>
	void Push(unsigned int count)
	{

	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_elements.push_back({GL_FLOAT, count, GL_FALSE});
		m_stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return m_elements; };
	inline unsigned int GetStride() const { return m_stride; };
};

