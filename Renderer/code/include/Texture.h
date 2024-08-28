#pragma once
#include "Renderer.h"

class Texture
{
private:
	unsigned int m_textureID, m_bindSlot;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_width, m_height, m_bytesPerPixel;

public:
	Texture(const std::string filePath);
	Texture(Texture&& other) noexcept;
	~Texture();

	void Bind(unsigned int slot = 0);
	void UnBind() const;

	inline int GetWidth() const { return m_width; };
	inline int GetHeight() const { return m_height; };
	inline unsigned int GetBindSlot() const { return m_bindSlot; };

};

