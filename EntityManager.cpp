#include "EntityManager.h"
#include <iostream>
#include "Collision.h"
#include "ColliderComponent.h"

void EntityManager::ClearData()
{
	for (auto& entity : entities)
	{
		entity->Destroy();
	}
}

bool EntityManager::HasNoEntities() const
{
	return entities.size() == 0;
}

Entity& EntityManager::AddEntity(std::string entityName, LayerType layer)
{
	Entity* entity = new Entity(*this, entityName, layer);
	entities.emplace_back(entity);

	return *entity;
}

Entity* EntityManager::GetEntityByName(std::string name) const
{
	for(auto& entity : entities)
	{
		if (entity->name.compare(name) == 0)
			return entity;
	}

	return NULL;
}


void EntityManager::Update(float deltaTime)
{
	for (int i = 0; i < entities.size(); i++)
	{
		if (!entities[i]->IsActive())
		{
			entities.erase(entities.begin() + i);
			continue;
		}
		entities[i]->Update(deltaTime);
	}
}

void EntityManager::Render()
{
	for (int layerNumber = 0; layerNumber < NUM_LAYERS; layerNumber++)
	{
		for (auto& entity : GetEntitiesByLayer(static_cast<LayerType>(layerNumber)))
		{
			entity->Render();
		}
	}
}

std::vector<Entity*> EntityManager::GetEntities() const
{
	return entities;
}

std::vector<Entity*> EntityManager::GetEntitiesByLayer(LayerType layer) const
{
	std::vector<Entity*> selectedEntities;
	for (auto& entity : entities)
	{
		if (entity->layer == layer)
			selectedEntities.emplace_back(entity);
	}
	return selectedEntities;
}

unsigned int EntityManager::GetEntityCount() const
{
	return entities.size();
}

void EntityManager::ListAllEntities() const
{
	unsigned int entityCount = 1;
	for (auto& entity : entities)
	{
		std::cout << "Entity [" << entityCount << "]:" << entity->name << std::endl;
		entity->ListAllComponents();
		entityCount++;
	}
}

CollisionType EntityManager::CheckCollisions()
{
	for(auto& firstEntity : entities)
	{ 
		if (!firstEntity->HasComponent<ColliderComponent>())
			continue;

		ColliderComponent* firstCollider = firstEntity->GetComponent<ColliderComponent>();

		for (auto& secondEntity : entities)
		{
			if (secondEntity->name.compare(firstEntity->name) == 0 || secondEntity->name.compare("Tile") == 0 
				|| !secondEntity->HasComponent<ColliderComponent>())
				continue;

			ColliderComponent* secondCollider = secondEntity->GetComponent<ColliderComponent>();
			if (Collision::CheckRectangleCollision(firstCollider->collider, secondCollider->collider))
			{
				if (firstCollider->colliderTag.compare("PLAYER") == 0 &&
					secondCollider->colliderTag.compare("ENEMY") == 0)
					return PLAYER_ENEMY_COLLISION;
				else if (firstCollider->colliderTag.compare("PLAYER") == 0 &&
					secondCollider->colliderTag.compare("PROJECTILE") == 0)
					return PLAYER_PROJECTILE_COLLISION;
				else if (firstCollider->colliderTag.compare("ENEMY") == 0 &&
					secondCollider->colliderTag.compare("FRIENDLY_PROJECTILE") == 0) 
				{
					firstEntity->Destroy();
					secondEntity->Destroy();
					return ENEMY_PROJECTILE_COLLISION;
				}
					
				else if (firstCollider->colliderTag.compare("PLAYER") == 0 &&
					secondCollider->colliderTag.compare("LEVEL_COMPLETE") == 0)
					return PLAYER_LEVEL_COMPLETE_COLLISION;
			}

		}
	}
	return NO_COLLISION;
}

std::string EntityManager::CheckEntityCollisions(Entity& entity) const
{
	ColliderComponent* collider = entity.GetComponent<ColliderComponent>();

	for (auto& otherEntity : entities)
	{
		if (otherEntity->name.compare(entity.name) == 0 || otherEntity->name.compare("Tile") == 0)
			continue;

		if (otherEntity->HasComponent<ColliderComponent>())
		{
			ColliderComponent* otherCollider = otherEntity->GetComponent<ColliderComponent>();
			if (Collision::CheckRectangleCollision(collider->collider, otherCollider->collider))
				return otherEntity->GetComponent<ColliderComponent>()->colliderTag;
		}
	}

	return std::string();
}