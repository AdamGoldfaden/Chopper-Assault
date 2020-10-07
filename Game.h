#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "sol.hpp"

#include "Entity.h"
#include "EntityManager.h"
#include "Component.h"

class AssetManager;
class Game
{
private:
	bool isRunning;
	SDL_Window* window;
	
public:
	Game();
	~Game();
	int ticksLastFrame;
	bool IsRunning() const;

	static SDL_Renderer* renderer;
	static AssetManager* assetManager;
	static SDL_Event event;
	static SDL_Rect camera;
	
	void Initialize(int width, int height);
	void ProcessInput();
	void Update();
	void Render();
	void Destroy();
	void LoadLevel(int levelNumber);
	void HandleCameraMovement();
	void CheckCollisions();
	void ProcessGameOver();
	void ProcessNextLevel(int levelNumber);
};
