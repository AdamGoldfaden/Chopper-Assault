#pragma once

#include "SDL.h"
#include "TextureManager.h"
#include "AssetManager.h"
#include "Animation.h"

class SpriteComponent : public Component
{
	private:
		SDL_Texture* texture;
		SDL_Rect sourceRectangle;
		SDL_Rect destinationRectangle;
		TransformComponent* transform;
		bool isAnimated;
		int numFrames;
		int animationSpeed;
		bool isFixed;
		std::map<std::string, Animation> animations;
		std::string currentAnimationName;
		unsigned int animationIndex = 0;

	public:
		SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

		SpriteComponent(std::string assetTextureID)
		{
			SetTexture(assetTextureID);
			isAnimated = false;
			isFixed = false;
		}

		SpriteComponent(std::string id, int numFrames, int anamationSpeed, bool hasDirections, bool isFixed)
		{
			isAnimated = true;
			this->numFrames = numFrames;
			this->animationSpeed = animationSpeed;
			this->isFixed = isFixed;

			if (hasDirections)
			{
				Animation downAnimation = Animation(0, numFrames, anamationSpeed);
				Animation rightAnimation = Animation(1, numFrames, anamationSpeed);
				Animation leftAnimation = Animation(2, numFrames, anamationSpeed);
				Animation upAnimation = Animation(3, numFrames, anamationSpeed);
				
				animations.emplace("DownAnimation", downAnimation);
				animations.emplace("RightAnimation", rightAnimation);
				animations.emplace("LeftAnimation", leftAnimation);
				animations.emplace("UpAnimation", upAnimation);

				animationIndex = 0;
				currentAnimationName = "DownAnimation";
			}
			else
			{
				Animation singleAnimation = Animation(0, numFrames, anamationSpeed);
				animations.emplace("SingleAnimation", singleAnimation);
				animationIndex = 0;
				currentAnimationName = "SingleAnimation";
			}

			SetTexture(id);
			Play(currentAnimationName);

		}

		void Play(std::string animationName)
		{
			numFrames = animations[animationName].numFrames;
			animationIndex = animations[animationName].index;
			animationSpeed = animations[animationName].animationSpeed;
			currentAnimationName = animationName;
		}

		void SetTexture(std::string assetTextureID)
		{
			texture = Game::assetManager->GetTexture(assetTextureID);
		}

		void Initialize() override
		{
			transform = owner->GetComponent<TransformComponent>();
			sourceRectangle.x = 0; 
			sourceRectangle.y = 0;
			sourceRectangle.w = transform->width;
			sourceRectangle.h = transform->height;
		}

		void Update(float deltaTime) override
		{
			if (isAnimated)
			{
				sourceRectangle.x = sourceRectangle.w * static_cast<int>((SDL_GetTicks() / animationSpeed) % numFrames);
			}
			sourceRectangle.y = animationIndex * transform->height;

			destinationRectangle.x = static_cast<int>(transform->position.x) - (isFixed ? 0 : Game::camera.x);
			destinationRectangle.y = static_cast<int>(transform->position.y) - (isFixed ? 0 : Game::camera.y);
			destinationRectangle.w = transform->width * transform->scale;
			destinationRectangle.h = transform->height * transform->scale;
			
		}

		void Render() override
		{
			TextureManager::Draw(texture, sourceRectangle, destinationRectangle, spriteFlip);
		}
};