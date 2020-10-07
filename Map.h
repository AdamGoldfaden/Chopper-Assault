#pragma once

#include <string>

class Map
{
	private:
		std::string textureID;
		int scale;
		int tileSize;

	public:
		Map(std::string textureID, int scale, int tileSize);
		~Map();
		void LoadMap(std::string filePath, int mapSizeX, int mapSizeY);
		void AddTile(int sourceX, int sourceY, int x, int y);

};