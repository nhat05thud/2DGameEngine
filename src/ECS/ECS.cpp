#include "ECS.h"
#include <../Logger/Logger.h>

int IComponent::nextId = 0;

int Entity::GetId() const {
	return id;
}

void Entity::Kill() {
	registry->KillEntity(*this);
}

void Entity::Tag(const std::string& tag) {
	registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const {
	return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group) {
	registry->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const {
	return registry->EntityBelongsToGroup(*this, group);
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

	if (freeIds.empty()) {
		// if there are no free ids waiting to be reused
		entityId = numEntities++;
		if (entityId >= entityComponentSignatures.size()) {
			entityComponentSignatures.resize(entityId + 1);
		}
	}
	else {
		// Reuse an id from the list of previously removed entities
		entityId = freeIds.front();
		freeIds.pop_front();
	}

	Entity entity(entityId);
	entity.registry = this;
	entitiesToBeAdded.insert(entity);

	Logger::Log("Entities created with id = " + std::to_string(entityId));
	return entity;
}

void Registry::KillEntity(Entity entity) {
	entitiesToBeKilled.insert(entity);
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

void Registry::RemoveEntityFromSystems(Entity entity) {
	for (auto system : systems) {
		system.second->RemoveEntityFromSysten(entity);
	}
}

void Registry::TagEntity(Entity entity, const std::string& tag) {
	entityPerTag.emplace(tag, entity);
	tagPerEntity.emplace(entity.GetId(), tag);
}

bool Registry::EntityHasTag(Entity entity, const std::string& tag) const {
	if (tagPerEntity.find(entity.GetId()) == tagPerEntity.end()) {
		return false;
	}
	return entityPerTag.find(tag)->second == entity;
}

Entity Registry::GetEntityByTag(const std::string& tag) const {
	return entityPerTag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity) {
	auto taggedEntity = tagPerEntity.find(entity.GetId());
	if (taggedEntity != tagPerEntity.end()) {
		auto tag = taggedEntity->second;
		entityPerTag.erase(tag);
		tagPerEntity.erase(taggedEntity);
	}
}

void Registry::GroupEntity(Entity entity, const std::string& group) {
	entitiesPerGroup.emplace(group, std::set<Entity>());
	entitiesPerGroup[group].emplace(entity);
	groupPerEntity.emplace(entity.GetId(), group);
}

bool Registry::EntityBelongsToGroup(Entity entity, const std::string& group) const {
	//// consider this validation => ???? EP107
	if (entitiesPerGroup.find(group) == entitiesPerGroup.end()) {
		return false;
	}
	////
	auto groupEntities = entitiesPerGroup.at(group);
	return groupEntities.find(entity.GetId()) != groupEntities.end();
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) const {
	auto& setOfEntities = entitiesPerGroup.at(group);
	return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end());
}

void Registry::RemoveEntityGroup(Entity entity) {
	// If in group, remove entity from group management
	auto groupedEntity = groupPerEntity.find(entity.GetId());
	if (groupedEntity != groupPerEntity.end()) {
		auto group = entitiesPerGroup.find(groupedEntity->second);
		if (group != entitiesPerGroup.end()) {
			auto entitiesInGroup = group->second.find(entity);
			if (entitiesInGroup != group->second.end()) {
				group->second.erase(entitiesInGroup);
			}
		}
		groupPerEntity.erase(groupedEntity);
	}
}

void Registry::Update() {
	// TODO: Processing the entities that are waitung to be created to the active Systems
	for (auto entity: entitiesToBeAdded) {
		AddEntityToSystems(entity);
	}
	entitiesToBeAdded.clear();

	// TODO: Process the entities that are waiting to be killed from the active Systems
	for (auto entity: entitiesToBeKilled) {
		RemoveEntityFromSystems(entity);
		entityComponentSignatures[entity.GetId()].reset();

		// Remove the entity from the component pools
		for (auto pool: componentPools) {
			if (pool) {
				pool->RemoveEntityFromPool(entity.GetId());
			}
		}
		
		// Make the entity id available to be reused
		freeIds.push_back(entity.GetId());

		// Remove any traces of that entity from the tag/group maps
		RemoveEntityTag(entity);
		RemoveEntityGroup(entity);
	}
	entitiesToBeKilled.clear();
}

