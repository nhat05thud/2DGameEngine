#include "ECS.h"
#include <../Logger/Logger.h>

int IComponent::nextId = 0;

int Entity::GetId() const {
	return id;
}

void System::AddEntityToSystem(Entity entity) {
	entities.push_back(entity);
}

void System::RemoveEntityFromSysten(Entity entity) {
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity other) {
		return entity == other;
	}), entities.end());
}

std::vector<Entity> System::GetSystemEntities() const {
	return entities;
}

const Signature& System::GetComponentSignature() const {
	return componentSignature;
}

Entity Registry::CreateEntity() {
	int entityId;
	entityId = numEntities++;

	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	// Make sure the entityComponentSignature vector can accomodate the new entity
	if (entityId >= entityComponentSignatures.size()) {
		entityComponentSignatures.resize(entityId + 1);
	}

	Logger::Log("Entities created with id = " + std::to_string(entityId));
	return entity;
}

void Registry::AddEntityToSystems(Entity entity) {
	const auto entityId = entity.GetId();

	// TODO: Match entityComponentSignature <---> systemComponentSignature
	const auto& entityComponentSignature = entityComponentSignatures[entityId];

	// Loop all the systems
	for (auto& system: systems) {
		const auto& systemComponentSignature = system.second->GetComponentSignature();
		// bitwise operator
		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;
		if (isInterested) {
			// TODO: Add the entity to the system
			system.second->AddEntityToSystem(entity);
		}
	}
}

void Registry::Update() {
	// TODO: Add the entities that are waitung to be created to the active Systems
	for (auto entity: entitiesToBeAdded) {
		AddEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();
	// TODO: Remove the entities that are waiting to be killed from the active Systems

}

