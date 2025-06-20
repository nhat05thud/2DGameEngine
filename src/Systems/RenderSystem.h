#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"
#include <SDL.h>
#include <algorithm>

class RenderSystem: public System {
	public:
		RenderSystem() {
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
		}

		void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, SDL_Rect& camera) {
			// TODO: Sort all the entities of our system by z-index
			struct RenderableEntity {
				TransformComponent transformComponent;
				SpriteComponent spriteComponent;
			};
			std::vector<RenderableEntity> renderableEntities;
			for (auto entity: GetSystemEntities()) {
				RenderableEntity renderableEntity;
				renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
				renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();

				// Check if the entities sprite is outside the camera view
				bool isEntityOutsideCamaraView = (
					renderableEntity.transformComponent.position.x + (renderableEntity.transformComponent.scale.x * renderableEntity.spriteComponent.width) < camera.x ||
					renderableEntity.transformComponent.position.x > camera.x + camera.w ||
					renderableEntity.transformComponent.position.y + (renderableEntity.transformComponent.scale.y * renderableEntity.spriteComponent.height) < camera.y ||
					renderableEntity.transformComponent.position.y > camera.y + camera.h
				);

				// Cull sprites that are outside the camera view (and are not fixed)
				if (isEntityOutsideCamaraView && !renderableEntity.spriteComponent.isFixed) {
					continue;
				}

				renderableEntities.emplace_back(renderableEntity);
			}

			// sort the vector by z-index value
			std::sort(renderableEntities.begin(), renderableEntities.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
				return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
			});

			// Loop all entities that the system is interested in
			for (auto entity : renderableEntities) {
				// TODO: Update entity position beased on its velocity
				const auto transform = entity.transformComponent;
				const auto sprite = entity.spriteComponent;
				// TODO: draw rectangle
				/*SDL_Rect objRect = {
					static_cast<int>(transform.position.x),
					static_cast<int>(transform.position.y),
					sprite.width,
					sprite.height
				};

				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderFillRect(renderer, &objRect);*/
				
				// TODO: Draw the PNG texture
				// set the source reactangle of our original sprite texture
				SDL_Rect srcRect = sprite.srcRect;

				// set the destination rectangle with the x, y position to be rendered
				SDL_Rect dstRect = { 
					static_cast<int>(transform.position.x - (sprite.isFixed ? 0 : camera.x)),
					static_cast<int>(transform.position.y - (sprite.isFixed ? 0 : camera.y)),
					static_cast<int>(sprite.width * transform.scale.x),
					static_cast<int>(sprite.height * transform.scale.y)
				};
				SDL_RenderCopyEx(
					renderer, 
					assetStore->GetTexture(sprite.assetId), 
					&srcRect, 
					&dstRect, 
					transform.rotation, 
					NULL, 
					sprite.flip
				);

				//Logger::Log("Entity id = " + std::to_string(entity.GetId()) + " position is now (" + std::to_string(transform.position.x) + "," + std::to_string(transform.position.y) + ")");
			}
		}
};

#endif // !RENDERSYSTEM_H
