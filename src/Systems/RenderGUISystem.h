#ifndef RENDERGUISYSTEM_H
#define RENDERGUISYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include <cmath>
#include <ImGui/imgui.h>
#include <ImGui/imgui_sdl.h>
#include <glm/glm.hpp>

class RenderGUISystem : public System {
	public:
		RenderGUISystem() = default;

		void Update(const std::unique_ptr<Registry>& registry, const SDL_Rect& camera) {
			ImGui::NewFrame();
			// TODO:
			//ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize;
			if (ImGui::Begin("Spawn enemies")) {
				// TODO: Input for the enemy X and Y position
				static int posX = 0;
				static int posY = 0;
				static int scaleX = 1;
				static int scaleY = 1;
				static int velX = 0;
				static int velY = 0;
				static int health = 100;
				static float rotation = 0.0;
				static float projAngle = 0.0;
				static float projSpeed = 0.0;
				static int projRepeat = 10;
				static int projDuration = 10;
				const char* sprites[] = { "tank-image", "truck-image" };
				static int selectedSpriteIndex = 0;

				// Selection to input enemy sprite
				if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::Combo("texture id", &selectedSpriteIndex, sprites, IM_ARRAYSIZE(sprites));
				}
				ImGui::Spacing();

				// selection to input enemy transform values
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::InputInt("position x", &posX);
					ImGui::InputInt("position y", &posY);
					ImGui::SliderInt("scale x", &scaleX, 1, 10);
					ImGui::SliderInt("scale y", &scaleY, 1, 10);
					ImGui::SliderAngle("rotation (deg)", &rotation, 0, 360);
				}
				ImGui::Spacing();

				// selection to input enemy rigid body values
				if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::InputInt("velocity x", &velX);
					ImGui::InputInt("velocity y", &velY);
				}
				ImGui::Spacing();

				// selection to input enemy rigid body values
				if (ImGui::CollapsingHeader("Projectile emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::SliderAngle("angle (deg)", &projAngle, 0, 360);
					ImGui::SliderFloat("speed (px/sec)", &projSpeed, 10, 500);
					ImGui::InputInt("repeat (sec)", &projRepeat);
					ImGui::InputInt("duration (sec)", &projDuration);
				}
				ImGui::Spacing();

				// selection to input enemy health values
				if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::SliderInt("%", &health, 0, 100);
				}
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				// Enemy creation button
				if (ImGui::Button("Spawn new enemy")) {
					Entity enemy = registry->CreateEntity();
					enemy.Group("enemies");
					enemy.AddComponent<TransformComponent>(glm::vec2(posX, posY), glm::vec2(scaleX, scaleY), rotation);
					enemy.AddComponent<RigidBodyComponent>(glm::vec2(velX, velY));
					enemy.AddComponent<SpriteComponent>(sprites[selectedSpriteIndex], 32, 32, 2, false);
					enemy.AddComponent<BoxColliderComponent>(25, 20, glm::vec2(5, 5));
					double projVelX = cos(projAngle) * projSpeed; // convert from angle-speed to x-value
					double projVelY = sin(projAngle) * projSpeed; // convert from angle-speed to y-value
					enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projVelX, projVelY), projRepeat * 1000, projDuration * 1000, 10, false);
					enemy.AddComponent<HealthComponent>(health);

					// Reset all input values after we create a new enemy;
					posX = posY = rotation = projAngle = 0;
					scaleX = scaleY = 1;
					projRepeat = projDuration = 10;
					projSpeed = 100;
					health = 100;
				}
			}
			ImGui::End();

			// Display a small overlay window to display the map position using the mouse
			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
			ImGui::SetNextWindowBgAlpha(0.9f);
			if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
				ImGui::Text(
					"Map coordinates (x=%.1f, y=%.1f)",
					ImGui::GetIO().MousePos.x + camera.x,
					ImGui::GetIO().MousePos.y + camera.y
				);
			}
			ImGui::End();

			ImGui::Render();
			ImGuiSDL::Render(ImGui::GetDrawData());
		}
};

#endif