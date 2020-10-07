#include "TextureManager.h"

SDL_Texture* TextureManager::LoadTexture(const char* fileName)
{
	SDL_Surface* surface = IMG_Load(fileName);
	if (surface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", fileName, IMG_GetError());

	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(Game::renderer, surface);
	if (texture == NULL)
	{
		printf("Unable to create texture from %s! SDL Error: %s\n", fileName, SDL_GetError());
	}

	SDL_FreeSurface(surface);
	return texture;
}

void TextureManager::Draw(SDL_Texture* texture, SDL_Rect sourceRectangle, SDL_Rect destinationRectangle, SDL_RendererFlip flip)
{
	SDL_RenderCopyEx(Game::renderer, texture, &sourceRectangle, &destinationRectangle, NULL, NULL, flip);
}