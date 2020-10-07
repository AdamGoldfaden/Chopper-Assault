#include "FontManager.h"

TTF_Font* FontManager::LoadFont(const char* fileName, int fontSize)
{
	return TTF_OpenFont(fileName, fontSize);
}

void FontManager::DrawFont(SDL_Texture* texture, SDL_Rect position)
{
	SDL_RenderCopy(Game::renderer, texture, NULL, &position);
}