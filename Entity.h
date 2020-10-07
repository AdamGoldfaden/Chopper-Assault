#pragma once

#include <vector>
#include <string>
#include <map>
#include <typeinfo>
#include "Component.h"
#include "EntityManager.h"
#include "Constants.h"

class EntityManager;
class Component;

class Entity
{
private:
	EntityManager& manager;
	bool isActive;
	std::vector<Component*> components;
	std::map<const std::type_info*, Component*> componentTypeMap;

public:
	std::string name;
	LayerType layer;
	Entity(EntityManager& manager);
	Entity(EntityManager& manager, std::string name, LayerType layer);
	void Update(float deltaTime);
	void Render();
	void Destroy();
	bool IsActive() const;
	void ListAllComponents() const;


	template <typename T, typename... TArgs> T& AddComponent(TArgs&&... args)
	{
		T* component(new T(std::forward<TArgs>(args)...));
		component->owner = this;
		components.emplace_back(component);
		componentTypeMap[&typeid(*component)] = component;
		component->Initialize();
		return *component;
	}

	template <typename T> T* GetComponent()
	{
		return static_cast<T*>(componentTypeMap[&typeid(T)]);
	}

	template <typename T> bool HasComponent() const
	{
		return componentTypeMap.count(&typeid(T));
	}
};