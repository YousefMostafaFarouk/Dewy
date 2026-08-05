#pragma once
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "Texture.h"
#include "Sprite.h"

class SpriteManager
{
public:
	int* m_textureSlots = NULL;
	int m_numberOfTextures = 0;

public:
	SpriteManager(std::vector<std::string> textureLocations);
	SpriteManager(std::map<std::string, std::string> nameTextureLocationMapping);
	~SpriteManager();

	void BindTextures();
	Sprite CreateSprite(std::string textureLocation, float xPos, float yPos, float size);
	Sprite CreateSprite(std::string textureLocation, float x1Pos, float y1Pos, float x2Pos, float y2Pos, float size, bool centered=true);
	int GetTextureSlot(std::string textureLocation);
	unsigned int GetTextureRendererId(const std::string& textureLocation) const;
private:
	std::unordered_map<std::string, int> m_textureId;
	std::vector<Texture> m_textures;
	std::vector<std::string> m_textureLocations;
};
