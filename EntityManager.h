#pragma once

#include "Entity.h"
#include "Component.h"
#include <vector>
#include "Constants.h"

class EntityManager
{
private:
	std::vector<Entity*> entities;

public:
	void ClearData();
	void Update(float deltaTime);
	void Render();
	bool HasNoEntities() const;
	Entity& AddEntity(std::string entityName, LayerType layer);
	Entity* GetEntityByName(std::string name) const;
	std::vector<Entity*> GetEntities() const;
	std::vector<Entity*> GetEntitiesByLayer(LayerType layer) const;
	unsigned int GetEntityCount() const;
	void ListAllEntities() const;
	std::string CheckEntityCollisions(Entity& entity) const;
	CollisionType CheckCollisions();
};