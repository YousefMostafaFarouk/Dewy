#include "Texture.h"
#include "../vendor/stb_image/stb_image.h"
#include "iostream"

Texture::Texture(const std::string filePath) 
	: m_textureID(0), m_filePath(filePath), m_localBuffer(nullptr), m_width(0),
	m_height(0),m_bytesPerPixel(0)
{
	std::cout << "created" << std::endl;
	stbi_set_flip_vertically_on_load(1);
	m_localBuffer = stbi_load(filePath.c_str(), &m_width, &m_height, &m_bytesPerPixel, 4);

	// Generates a 2d texture and binds it
	GLCall(glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_textureID));

	// Sets how a texture should be magninfied and minimized based on object it is on
	// i stands for integer
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	// Sets how the texture should wrap around an object horizontally (S) and vertically (T)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer));

	if (m_localBuffer)
		stbi_image_free(m_localBuffer);
	else
	{
		std::string errorMessage = "Invalid texture " + m_filePath;
		std::cerr << errorMessage << std::endl;
		throw std::invalid_argument(errorMessage.c_str());
	}

}

Texture::Texture(Texture&& other) noexcept
	: m_textureID(other.m_textureID), m_filePath(std::move(other.m_filePath)),
	m_localBuffer(other.m_localBuffer), m_width(other.m_width),
	m_height(other.m_height), m_bytesPerPixel(other.m_bytesPerPixel) {

	other.m_textureID = 0; 
	other.m_localBuffer = nullptr;
	other.m_width = 0;
	other.m_height = 0;
	other.m_bytesPerPixel = 0;
}


Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_textureID));
}

void Texture::Bind(unsigned int slot)
{
	m_bindSlot = slot;
	GLCall(glBindTexture(GL_TEXTURE_2D, m_textureID));
	GLCall(glBindTextureUnit(m_bindSlot, m_textureID));
}

void Texture::UnBind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

