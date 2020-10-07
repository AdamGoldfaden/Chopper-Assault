#include <iostream>
#include "glm.hpp"
#include "Constants.h"
#include <winsock2.h>

#include "Game.h"
#include "AssetManager.h"
#include "Map.h"

#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "KeyboardControlComponent.h"
#include "TextLabelComponent.h"
#include "ProjectileEmitterComponent.h"

EntityManager manager;
SDL_Renderer* Game::renderer;
AssetManager* Game::assetManager = new AssetManager(&manager);
SDL_Event Game::event;
SDL_Rect Game::camera = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
Map* map;

int playerHealth;
int score;

Game::Game()
{
	isRunning = false;
}

Game::~Game()
{}

bool Game::IsRunning() const
{
	return isRunning;
}

void Game::Initialize(int width, int height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << "Error initializing SDL" << std::endl;
		return;
	}

	if (TTF_Init() != 0)
	{
		std::cerr << "Error initializing SDL_TTF" << std::endl;
		return;
	}

	window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_BORDERLESS);
	if (!window)
	{
		std::cerr << "Error creating SDL window" << std::endl;
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer)
	{
		std::cerr << "Error creating SDL renderer" << std::endl;
		return;
	}

	LoadLevel(1);

	isRunning = true;
}


void Game::LoadLevel(int levelNumber)
{
	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math);

	std::string levelName = "Level" + std::to_string(levelNumber);
	lua.script_file("assets/scripts/" + levelName + ".lua");
	sol::table levelData = lua[levelName];

	/*---------------------------------------LOAD ASSETS-----------------------------------------------*/
	sol::table levelAssets = levelData["assets"];

	unsigned int assetIndex = 0;
	while (true)
	{
		sol::optional<sol::table> existsAssetIndexNode = levelAssets[assetIndex];
		if (existsAssetIndexNode == sol::nullopt)
			break;

		sol::table asset = levelAssets[assetIndex];

		std::string assetType = asset["type"];
		if (assetType.compare("texture") == 0)
		{
			std::string textureID = asset["id"];
			std::string textureFile = asset["file"];
			assetManager->AddTexture(textureID, textureFile.c_str());
		}
		else if (assetType.compare("font") == 0)
		{
			std::string fontID = asset["id"];
			std::string fontFile = asset["file"];
			int fontSize = asset["fontSize"];
			assetManager->AddFont(fontID, fontFile.c_str(), fontSize);
		}
		assetIndex++;
	}

	/*---------------------------------------LOAD MAP-----------------------------------------------*/
	sol::table levelMap = levelData["map"];
	std::string mapTextureID = levelMap["textureAssetId"];
	std::string mapFile = levelMap["file"];

	map = new Map(mapTextureID, levelMap["scale"], levelMap["tileSize"]);
	map->LoadMap(mapFile, levelMap["mapSizeX"], levelMap["mapSizeY"]);

	/*-----------------------------------LOAD ENTITIES & COMPONENTS-----------------------------------------------*/
	sol::table levelEntities = levelData["entities"];

	unsigned int entityIndex = 0;
	while(true)
	{ 
		sol::optional<sol::table> existsEntityIndexNode = levelEntities[entityIndex];
		if (existsEntityIndexNode == sol::nullopt)
			break;

		sol::table entity = levelEntities[entityIndex];
		std::string entityName = entity["name"];
		LayerType entityLayer = static_cast<LayerType>(entity["layer"]);

		Entity& newEntity = manager.AddEntity(entityName, entityLayer);
		sol::table entityComponents = entity["components"];

		sol::optional<sol::table> existsTransformComponent = entityComponents["transform"];
		if (existsTransformComponent != sol::nullopt)
		{
			newEntity.AddComponent<TransformComponent>(
				static_cast<int>(entityComponents["transform"]["position"]["x"]),
				static_cast<int>(entityComponents["transform"]["position"]["y"]),
				static_cast<int>(entityComponents["transform"]["velocity"]["x"]),
				static_cast<int>(entityComponents["transform"]["velocity"]["y"]),
				static_cast<int>(entityComponents["transform"]["width"]),
				static_cast<int>(entityComponents["transform"]["height"]),
				static_cast<int>(entityComponents["transform"]["scale"]));
		}

		sol::optional<sol::table> existsSpriteComponent = entityComponents["sprite"];
		if (existsSpriteComponent != sol::nullopt)
		{
			std::string spriteTextureID = entityComponents["sprite"]["textureAssetId"];
			bool spriteIsAnimated = static_cast<bool>(entityComponents["sprite"]["animated"]);
			if (spriteIsAnimated)
			{
				newEntity.AddComponent<SpriteComponent>(
					spriteTextureID,
					static_cast<int>(entityComponents["sprite"]["frameCount"]),
					static_cast<int>(entityComponents["sprite"]["animationSpeed"]),
					static_cast<bool>(entityComponents["sprite"]["hasDirections"]),
					static_cast<bool>(entityComponents["sprite"]["fixed"]));
			}
			else {
				newEntity.AddComponent<SpriteComponent>(spriteTextureID);
			}
		}

		sol::optional<sol::table> existsColliderComponent = entityComponents["collider"];
		if (existsColliderComponent != sol::nullopt)
		{
			std::string colliderTag = entityComponents["collider"]["tag"];
			newEntity.AddComponent<ColliderComponent>(colliderTag);
		}

		sol::optional<sol::table> existsKeyboardControlComponent = entityComponents["input"]["keyboard"];
		if (existsKeyboardControlComponent != sol::nullopt)
		{
			std::string upKey = entityComponents["input"]["keyboard"]["up"];
			std::string rightKey = entityComponents["input"]["keyboard"]["right"];
			std::string downKey = entityComponents["input"]["keyboard"]["down"];
			std::string leftKey = entityComponents["input"]["keyboard"]["left"];
			std::string shootKey = entityComponents["input"]["keyboard"]["shoot"];
			newEntity.AddComponent<KeyboardControlComponent>(upKey, rightKey, downKey, leftKey, shootKey);
		}

		sol::optional<sol::table> existsProjectileEmitterComponent = entityComponents["projectileEmitter"];
		if (existsProjectileEmitterComponent != sol::nullopt)
		{
			Entity& projectileEntity = manager.AddEntity(newEntity.name + "_projectile", PROJECTILE_LAYER);
			TransformComponent* parentTransform = newEntity.GetComponent<TransformComponent>();

			projectileEntity.AddComponent<TransformComponent>(
				static_cast<int>(parentTransform->position.x + (parentTransform->width / 2)),
				static_cast<int>(parentTransform->position.y + (parentTransform->height / 2)),
				0,
				0,
				static_cast<int>(entityComponents["projectileEmitter"]["width"]),
				static_cast<int>(entityComponents["projectileEmitter"]["height"]),
				1);

			std::string projectileSpriteTextureID = entityComponents["projectileEmitter"]["textureAssetId"];
			projectileEntity.AddComponent<SpriteComponent>(projectileSpriteTextureID);
			projectileEntity.AddComponent<ColliderComponent>("PROJECTILE");

			projectileEntity.AddComponent<ProjectileEmitterComponent>(
				static_cast<int>(entityComponents["projectileEmitter"]["speed"]),
				static_cast<int>(entityComponents["projectileEmitter"]["angle"]),
				static_cast<int>(entityComponents["projectileEmitter"]["range"]),
				static_cast<bool>(entityComponents["projectileEmitter"]["shouldLoop"]),
				&newEntity);
		}

		entityIndex++;
	}

	Entity& labelLevelName = manager.AddEntity("LabelLevelName", UI_LAYER);
	labelLevelName.AddComponent<TextLabelComponent>(10, 10, "First Level", "charriot-font", WHITE_COLOR);
}

void Game::ProcessInput()
{
	SDL_PollEvent(&event);
	switch (event.type)
	{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if(event.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
			}
			break;
		default:
			break;
	}
}

void Game::Update()
{
	int timeToWait = FRAME_TARGET_TIME - (SDL_GetTicks() - ticksLastFrame);
	if (timeToWait < FRAME_TARGET_TIME && timeToWait > 0){SDL_Delay(timeToWait);}

	float deltaTime = (SDL_GetTicks() - ticksLastFrame) / 1000.0f;
	if (deltaTime > 0.05f){deltaTime = 0.05f;}

	ticksLastFrame = SDL_GetTicks();

	manager.Update(deltaTime);

	HandleCameraMovement();
	CheckCollisions();
}

void Game::Render()
{
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	if (manager.HasNoEntities()) { return; }

	manager.Render();

	SDL_RenderPresent(renderer);
}

void Game::HandleCameraMovement()
{
	TransformComponent* mainPlayerTransform = manager.GetEntityByName("player")->GetComponent<TransformComponent>();

	camera.x = (int)(mainPlayerTransform->position.x - (WINDOW_WIDTH/2));
	camera.y = (int)(mainPlayerTransform->position.y - (WINDOW_HEIGHT/2));

	if (camera.x < 0)
		camera.x = 0;
	else if (camera.x > camera.w)
		camera.x = camera.w;
	if (camera.y < 0)
		camera.y = 0;
	else if (camera.y > camera.h)
		camera.y = camera.h;
}

void Game::CheckCollisions()
{
	CollisionType collisionType = manager.CheckCollisions();

	if (collisionType == PLAYER_ENEMY_COLLISION)
	{
		ProcessGameOver();
	}
	else if (collisionType == PLAYER_LEVEL_COMPLETE_COLLISION)
	{
		ProcessNextLevel(1);
	}
	else if (collisionType == PLAYER_PROJECTILE_COLLISION)
	{
		ProcessGameOver();
	}
	else if (collisionType == ENEMY_PROJECTILE_COLLISION)
	{

	}
}

void Game::ProcessNextLevel(int levelNumber)
{
	std::cout << "next level" << std::endl;
	isRunning = false;
}

void Game::ProcessGameOver()
{
	std::cout << "game over" << std::endl;
	isRunning = false;
}

void Game::Destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}