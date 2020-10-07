#pragma once

#include "Game.h"
#include "EntityManager.h"
#include "TransformComponent.h"
#include "TextureManager.h"
#include "AssetManager.h"

class ColliderComponent : public Component
{
	public:
		std::string colliderTag;
		SDL_Rect collider;
		SDL_Rect sourceRectangle;
		SDL_Rect destinationRectangle;
		TransformComponent* transform;
		SDL_Texture* texture;

		ColliderComponent(std::string colliderTag)
		{
			this->colliderTag = colliderTag;
		}

		void Initialize() override
		{
			if (owner->HasComponent<TransformComponent>())
			{
				transform = owner->GetComponent<TransformComponent>();
				sourceRectangle = { 0, 0, transform->width, transform->height };
				destinationRectangle = {collider.x, collider.y, collider.w, collider.h};

				collider.x = static_cast<int>(transform->position.x);
				collider.y = static_cast<int>(transform->position.y);
				collider.w = transform->width * transform->scale;
				collider.h = transform->height * transform->scale;
			}

			texture = Game::assetManager->GetTexture("collider-image");
		}

		void Update(float deltaTime) override
		{
			collider.x = static_cast<int>(transform->position.x);
			collider.y = static_cast<int>(transform->position.y);
			collider.w = transform->width * transform->scale;
			collider.h = transform->height * transform->scale;

			destinationRectangle.x = collider.x - Game::camera.x;
			destinationRectangle.y = collider.y - Game::camera.y;
		}

		/*void Render() override
		{
			TextureManager::Draw(texture, sourceRectangle, destinationRectangle, SDL_FLIP_NONE);
		}*/
};