#pragma once

#include "Game.h"
#include "EntityManager.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "ProjectileEmitterComponent.h"

extern EntityManager manager;

class KeyboardControlComponent : public Component
{
	private:
		bool canShoot;

	public:
		std::string upKey;
		std::string downKey;
		std::string rightKey;
		std::string leftKey;
		std::string shootKey;
		TransformComponent* transform;
		SpriteComponent* sprite;
		float movementSpeed = 100;
		Direction direction;

		KeyboardControlComponent();
		KeyboardControlComponent(std::string upKey, std::string rightKey, std::string downKey, 
			std::string leftKey, std::string shootKey)
		{
			this->upKey = GetSDLKeyStringCode(upKey);
			this->rightKey = GetSDLKeyStringCode(rightKey);
			this->downKey = GetSDLKeyStringCode(downKey);
			this->leftKey = GetSDLKeyStringCode(leftKey);
			this->shootKey = GetSDLKeyStringCode(shootKey);

			direction = DOWN;
			canShoot = true;
		}

		std::string GetSDLKeyStringCode(std::string key)
		{
			if (key.compare("up") == 0) return "1073741906";
			if (key.compare("down") == 0) return "1073741905";
			if (key.compare("left") == 0) return "1073741904";
			if (key.compare("right") == 0) return "1073741903";
			if (key.compare("space") == 0) return "32";
			return std::to_string(static_cast<int>(key[0]));
		}

		void Initialize() override
		{
			transform = owner->GetComponent<TransformComponent>();
			sprite = owner->GetComponent<SpriteComponent>();
		}

		void SpawnProjetile()
		{
			if (!canShoot)
				return;
			Entity& projectile = manager.AddEntity("player-projectile", PROJECTILE_LAYER);
			projectile.AddComponent<TransformComponent>(static_cast<int>(transform->position.x + transform->width / 2),
				static_cast<int>(transform->position.y + transform->height / 2), 0, 0, 4, 4, 1);
			projectile.AddComponent<SpriteComponent>("friendly-projectile-texture");
			projectile.AddComponent<ColliderComponent>("FRIENDLY_PROJECTILE");
			projectile.AddComponent<ProjectileEmitterComponent>(static_cast<int>(movementSpeed + 50), 
				static_cast<int>(direction) * 90, 300, false, owner);
		}

		void Update(float deltaTime) override
		{
			std::string keyCode = std::to_string(Game::event.key.keysym.sym);

			if (Game::event.type == SDL_KEYDOWN)
			{
				if (keyCode.compare(upKey) == 0)
				{
					/*if (transform->position.y <= 0)
					{
						transform->position.y = 0;
						return;
					}*/
					transform->velocity.y = -movementSpeed;
					transform->velocity.x = 0;
					sprite->Play("UpAnimation");
					direction = UP;
					
				}
				else if (keyCode.compare(rightKey) == 0)
				{
					/*if (transform->position.x >= WINDOW_WIDTH - transform->width)
					{
						transform->position.x = (float)(WINDOW_WIDTH - transform->width);
						return;
					}*/
					transform->velocity.y = 0;
					transform->velocity.x = movementSpeed;
					sprite->Play("RightAnimation");
					direction = RIGHT;
				}
				else if (keyCode.compare(leftKey) == 0)
				{
					/*if (transform->position.x <= 0)
					{
						transform->position.x = 0;
						return;
					}*/
					transform->velocity.y = 0;
					transform->velocity.x = -movementSpeed;
					sprite->Play("LeftAnimation");
					direction = LEFT;
				}
				else if (keyCode.compare(downKey) == 0)
				{
					/*if (transform->position.y >= WINDOW_HEIGHT - transform->height)
					{
						transform->position.y = (float)(WINDOW_HEIGHT - transform->height);
						return;
					}*/
					transform->velocity.y = movementSpeed;
					transform->velocity.x = 0;
					sprite->Play("DownAnimation");
					direction = DOWN;
				}
				else if (keyCode.compare(shootKey) == 0)
				{
					SpawnProjetile();
					canShoot = false;
				}
			}

			if (Game::event.type == SDL_KEYUP)
			{
				if (keyCode.compare(upKey) == 0)
				{
					if(transform->velocity.y < 0)
						transform->velocity.y = 0;
				}
				else if (keyCode.compare(rightKey) == 0)
				{
					if(transform->velocity.x > 0)
						transform->velocity.x = 0;
				}
				else if (keyCode.compare(leftKey) == 0)
				{
					if(transform->velocity.x < 0)
						transform->velocity.x = 0;
				}
				else if (keyCode.compare(downKey) == 0)
				{
					if(transform->velocity.y > 0)
						transform->velocity.y = 0;
				}
				else if (keyCode.compare(shootKey) == 0)
				{
					canShoot = true;
				}

			}
		}
};