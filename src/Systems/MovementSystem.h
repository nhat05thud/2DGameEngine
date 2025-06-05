#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
//#include <glm/glm.hpp>

class MovementSystem : public System {
	public:
		MovementSystem() {
			// TODO: 
			RequireComponent<TransformComponent>();
			RequireComponent<RigidBodyComponent>();

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

				//Logger::Log("Entity id = " + std::to_string(entity.GetId()) + " position is now (" + std::to_string(transform.position.x) + "," + std::to_string(transform.position.y) + ")");
			}
		}
};

#endif // !MOVEMENTSYSTEM_H
