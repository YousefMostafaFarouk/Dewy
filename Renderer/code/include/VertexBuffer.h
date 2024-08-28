#pragma once
class VertexBuffer
{
private:
	unsigned int m_rendererID;
	unsigned int m_numberOfVerticies{};
public:
	VertexBuffer(const void* data, unsigned int size, unsigned int numberOfVerticies);
	VertexBuffer();
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;
	void AddBufferData(unsigned int size, const void* data, unsigned int numberOfVerticies);
	inline unsigned int GetVerticiesCount() const { return m_numberOfVerticies; };


};

