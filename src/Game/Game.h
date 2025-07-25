#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"
#include <SDL.h>
#include <sol/sol.hpp>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS; // 1s = 1000ms

class Game {
	private:
		bool isRunning;
		bool isDebug;
		int millisecsPreviousFrame = 0;
		SDL_Window* window;
		SDL_Renderer* renderer;
		SDL_Rect camera;

		sol::state lua;

		// use smart pointer
		std::unique_ptr<Registry> registry; //Registry* registry
		std::unique_ptr<AssetStore> assetStore;
		std::unique_ptr<EventBus> eventBus;

	public:
		Game();
		~Game();
		void Initialize();
		void Run();
		void Setup();
		void ProcessInput();
		void Update();
		void Render();
		void Destroy();

		static int windowWidth;
		static int windowHeight;
		static int mapWidth;
		static int mapHeight;
};
#endif 