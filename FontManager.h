#pragma once

#include "Game.h"

class FontManager
{
	public:
		static TTF_Font* LoadFont(const char* fileName, int fontSize);
		static void DrawFont(SDL_Texture* texture, SDL_Rect position);
};