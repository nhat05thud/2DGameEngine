#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Game/Game.h"
#include "../EventBus/Event.h"
#include "../Events/CollisionEvent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
//#include <glm/glm.hpp>

class MovementSystem : public System {
	public:
		MovementSystem() {
			// TODO: 
			RequireComponent<TransformComponent>();
			RequireComponent<RigidBodyComponent>();

		}

		void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
			eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
		}

		void OnCollision(CollisionEvent& event) {
			Entity a = event.a;
			Entity b = event.b;
			Logger::Log("Collision event emitted: " + std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));

			if (a.BelongsToGroup("enemies") && b.BelongsToGroup("obstacles")) {
				OnEnemyHitsObstacle(a, b); // "a" is the enemy, "b" is the obstacle
			}
			if (a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies")) {
				OnEnemyHitsObstacle(b, a); // "a" is the obstacle, "b" is the enemy
			}
		}

		void OnEnemyHitsObstacle(Entity enemy, Entity obstacle) {
			if (enemy.HasComponent<RigidBodyComponent>() && enemy.HasComponent<SpriteComponent>()) {
				auto& rigidbody = enemy.GetComponent<RigidBodyComponent>();
				auto& sprite = enemy.GetComponent<SpriteComponent>();

				if (rigidbody.velocity.x != 0) {
					rigidbody.velocity.x *= -1;
					sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
				}
				if (rigidbody.velocity.y != 0) {
					rigidbody.velocity.y *= -1;
					sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
				}
			}
		}

		void Update(double deltaTime) {
			// TODO
			// Loop all entities that the system is interested in
			for (auto entity : GetSystemEntities()) {
				// TODO: Update entity position beased on its velocity
				auto& transform = entity.GetComponent<TransformComponent>();
				const auto rigidbody = entity.GetComponent<RigidBodyComponent>();

				transform.position.x += rigidbody.velocity.x * deltaTime;
				transform.position.y += rigidbody.velocity.y * deltaTime;

				// Prevent the main player from moving outside the map boundaries
				if (entity.HasTag("player")) {
					int paddingLeft = 10;
					int paddingTop = 10;
					int paddingRight = 50;
					int paddingBottom = 50;
					transform.position.x = transform.position.x < paddingLeft ? paddingLeft : transform.position.x;
					transform.position.x = transform.position.x > Game::mapWidth - paddingRight ? Game::mapWidth - paddingRight : transform.position.x;
					transform.position.y = transform.position.y < paddingLeft ? paddingTop : transform.position.y;
					transform.position.y = transform.position.y > Game::mapHeight - paddingBottom ? Game::mapHeight - paddingBottom : transform.position.y;
				}

				// check entity outside the map
				bool isEntityOutsideMap = (
					transform.position.x < 0 ||
					transform.position.x > Game::mapWidth ||
					transform.position.y < 0 ||
					transform.position.y > Game::mapHeight
					);

				// Kill all entities that move outside the map boundaries
				if (isEntityOutsideMap && !entity.HasTag("player")) {
					entity.Kill();
				}
				//Logger::Log("Entity id = " + std::to_string(entity.GetId()) + " position is now (" + std::to_string(transform.position.x) + "," + std::to_string(transform.position.y) + ")");
			}
		}
};

#endif // !MOVEMENTSYSTEM_H
