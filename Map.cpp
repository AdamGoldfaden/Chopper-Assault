#include "Map.h"
#include "Game.h"
#include "EntityManager.h"
#include "TileComponent.h"
#include <fstream>


extern EntityManager manager;

Map::Map(std::string textureID, int scale, int tileSize)
{
	this->textureID = textureID;
	this->scale = scale;
	this->tileSize = tileSize;
}

void Map::LoadMap(std::string filePath, int mapSizeX, int mapSizeY)
{
	std::fstream mapFile;
	mapFile.open(filePath);

	for (int y = 0; y < mapSizeY; y++)
	{
		for (int x = 0; x < mapSizeX; x++)
		{
			char ch;

			mapFile.get(ch);
			int sourceRectY = atoi(&ch) * tileSize;
			mapFile.get(ch);
			int sourceRectX = atoi(&ch) * tileSize;

			AddTile(sourceRectX, sourceRectY, x * (scale * tileSize), y * (scale * tileSize));

			mapFile.ignore();
		}
	}

	mapFile.close();
}

void Map::AddTile(int sourceX, int sourceY, int x, int y)
{
	Entity& newTile = manager.AddEntity("Tile", TILEMAP_LAYER);
	newTile.AddComponent<TileComponent>(sourceX, sourceY, x, y, tileSize, scale, textureID);
}

