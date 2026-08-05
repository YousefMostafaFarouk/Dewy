#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"
#include <cstdint>

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_rendererID));
	GLCall(glBindVertexArray(m_rendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_rendererID));
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_rendererID));
}

void VertexArray::UnBind() const
{
	GLCall(glBindVertexArray(0));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); ++i)
	{
		const auto& element = elements[i];

		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(),
			reinterpret_cast<const void*>(static_cast<std::uintptr_t>(offset))));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}
