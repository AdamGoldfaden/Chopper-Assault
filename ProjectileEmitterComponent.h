#pragma once

#include "glm.hpp"
#include "EntityManager.h"
#include "TransformComponent.h"

class ProjectileEmitterComponent : public Component
{
	private:
		TransformComponent* transform;
		glm::vec2 origin;
		int speed;
		int range;
		float angleRad;
		bool shouldLoop;

	public:
		Entity* parentEntity;

		ProjectileEmitterComponent(int speed, int angleDeg, int range, bool shouldLoop, Entity* parentEntity)
		{
			this->speed = speed;
			this->range = range;
			angleRad = glm::radians(static_cast<float>(angleDeg));
			this->shouldLoop = shouldLoop;
			this->parentEntity = parentEntity;
		}

		void Initialize() override
		{
			transform = owner->GetComponent<TransformComponent>();
			origin = glm::vec2(transform->position.x, transform->position.y);
			transform->velocity = glm::vec2(speed * glm::cos(angleRad), speed * glm::sin(angleRad));
		}

		void Update(float deltaTime) override
		{
			if (!parentEntity->IsActive())
				owner->Destroy();

			if (glm::distance(transform->position, origin) > range)
			{
				if (shouldLoop)
				{
					transform->position.x = origin.x;
					transform->position.y = origin.y;
				}
				else
				{
					owner->Destroy();
				}
			}
		}
};