#pragma once
#include <type_traits>
#include <vector>
#include "Renderer.h"

struct VertexBufferElement
{
	unsigned int count;
	unsigned int type;
	unsigned char normalized;

	VertexBufferElement(unsigned int t, unsigned int c, bool n) :
		count(c), type(t), normalized(n)
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
		if constexpr (std::is_same_v<T, float>)
		{
			m_elements.push_back({ GL_FLOAT, count, GL_FALSE });
			m_stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
		}
		else if constexpr (std::is_same_v<T, unsigned int>)
		{
			m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
			m_stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
		}
		else if constexpr (std::is_same_v<T, unsigned char>)
		{
			m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
			m_stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
		}
		else
		{
			static_assert(!sizeof(T), "Unsupported vertex attribute type");
		}
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return m_elements; };
	inline unsigned int GetStride() const { return m_stride; };
};
