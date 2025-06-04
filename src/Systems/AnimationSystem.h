#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include <SDL.h>

class AnimationSystem : public System {
	public:
		AnimationSystem() {
			// TODO: 
			RequireComponent<SpriteComponent>();
			RequireComponent<AnimationComponent>();
		}
		void Update() {
			for (auto entity: GetSystemEntities()) {
				auto& animation = entity.GetComponent<AnimationComponent>();
				auto& sprite = entity.GetComponent<SpriteComponent>();

				// TODO: Change the current frame
				animation.currentFrame = ((SDL_GetTicks() - animation.startTime) * animation.frameSpeedRate / 1000) % animation.numFrames;
				sprite.srcRect.x = animation.currentFrame * sprite.width;
				//sprite.srcRect.y = animation.currentFrame * sprite.height;
			}
		}
};

#endif // !ANIMATIONSYSTEM_H
