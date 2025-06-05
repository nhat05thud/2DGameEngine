#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include <SDL.h>

class CollisionSystem : public System {
	public:
		CollisionSystem() {
			RequireComponent<TransformComponent>();
			RequireComponent<BoxColliderComponent>();
		}
		void Update(std::unique_ptr<EventBus>& eventBus) {
			// TODO:
			// Check all entities that have a boxcollider
			// to see if they are colliding with each other
			// Tip: AABB Collision check
			// O(n²) => big O notation
			auto entities = GetSystemEntities();
			// Loop all the entities that the system is enterested in
			for (auto i = entities.begin(); i != entities.end(); i++) {
				Entity a = *i;
				auto aTransform = a.GetComponent<TransformComponent>();
				auto aCollider = a.GetComponent<BoxColliderComponent>();
				
				// Loop all the entities that still need to be checked (to the right of i)
				for (auto j = i; j != entities.end(); j++) {
					Entity b = *j;

					// bypass if we are trying to test the same entity
					if (a == b) {
						continue;
					}

					auto bTransform = b.GetComponent<TransformComponent>();
					auto bCollider = b.GetComponent<BoxColliderComponent>();

					// Perform the AABB collistion check between entities a and b
					bool collisionHappened = CheclAABBCollision(
						aTransform.position.x + aCollider.offset.x,
						aTransform.position.y + aCollider.offset.y,
						aCollider.width,
						aCollider.height,
						bTransform.position.x + bCollider.offset.x,
						bTransform.position.y + bCollider.offset.y,
						bCollider.width,
						bCollider.height
					);

					if (collisionHappened) {
						Logger::Log("Entity " + std::to_string(a.GetId()) + "is colliding with Entity " + std::to_string(b.GetId()));

						// TODO: emit an event
						eventBus->EmitEvent<CollisionEvent>(a, b);
					}
				}
			}
		}

		bool CheclAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
			return (
				aX < bX + bW &&
				aX + aW > bW &&
				aY < bY + bH &&
				aY + aH > bY
			);
		}
};

#endif // !COLLISIONSYSTEM_H
