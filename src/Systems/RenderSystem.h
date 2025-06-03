#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"
#include <SDL.h>

class RenderSystem: public System {
	public:
		RenderSystem() {
			RequireComponent<TransformComponent>();
			RequireComponent<SpriteComponent>();
		}

		void Update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore) {
			for (auto entity : GetSystemEntities()) {
				// TODO: Update entity position beased on its velocity
				const auto transform = entity.GetComponent<TransformComponent>();
				const auto sprite = entity.GetComponent<SpriteComponent>();
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
