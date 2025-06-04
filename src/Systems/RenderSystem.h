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

		void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {
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
					static_cast<int>(transform.position.x),
					static_cast<int>(transform.position.y),
					static_cast<int>(sprite.width * transform.scale.x),
					static_cast<int>(sprite.height * transform.scale.y)
				};
				SDL_Texture* texture = assetStore-> GetTexture(sprite.assetId);
				SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, transform.rotation, NULL, SDL_FLIP_NONE);

				//Logger::Log("Entity id = " + std::to_string(entity.GetId()) + " position is now (" + std::to_string(transform.position.x) + "," + std::to_string(transform.position.y) + ")");
			}
		}
};

#endif // !RENDERSYSTEM_H
