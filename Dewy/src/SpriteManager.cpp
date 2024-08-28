#include "SpriteManager.h"
#include "iostream"

SpriteManager::SpriteManager(std::vector<std::string> textureLocations) : m_textureLocations(textureLocations)
{
	numberOfTextures = m_textureLocations.size();
	textureSlots = new int[numberOfTextures];

	for (int i = 0; i < numberOfTextures; ++i)
		textureSlots[i] = i + 1;
}

SpriteManager::SpriteManager(std::map<std::string, std::string> nameTextureLocationMapping)
{
	for (auto pair : nameTextureLocationMapping)
	{
		m_textureLocations.push_back(pair.second);
	}

	numberOfTextures = m_textureLocations.size();
	textureSlots = new int[numberOfTextures];

	for (int i = 0; i < numberOfTextures; ++i)
		textureSlots[i] = i + 1;
}

SpriteManager::~SpriteManager()
{
	delete [] textureSlots;
}

void SpriteManager::BindTextures()
{
	for (int i = 0; i < m_textureLocations.size(); ++i)
	{
		m_textureId[m_textureLocations[i]] = i + 1;
		m_textures.push_back(Texture(m_textureLocations[i]));
		m_textures[i].Bind(i + 1);
	}
}

Sprite SpriteManager::CreateSprite(std::string textureLocation, float xPos, float yPos, float size)
{
	return Sprite(xPos, yPos, size, m_textureId[textureLocation]);
}

Sprite SpriteManager::CreateSprite(std::string textureLocation, float x1Pos, float y1Pos, float x2Pos, float y2Pos, float size, bool centered)
{
	return Sprite(x1Pos, y1Pos, x2Pos, y2Pos, size, m_textureId[textureLocation], centered);
}

int SpriteManager::GetTextureSlot(std::string textureLocation)
{
	if (!m_textureId[textureLocation])
	{
		std::string errorMessage = "Can't get texture location " + textureLocation;
		std::cerr << errorMessage << std::endl;
		throw std::invalid_argument(errorMessage.c_str());
	}
	return m_textureId[textureLocation];
}
