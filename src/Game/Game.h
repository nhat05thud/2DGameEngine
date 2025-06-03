#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include <SDL.h>
//#include <memory>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS; // 1s = 1000ms

class Game {
	private:
		bool isRunning;
		int millisecsPreviousFrame = 0;
		SDL_Window* window;
		SDL_Renderer* renderer;

		// use smart pointer
		std::unique_ptr<Registry> registry; //Registry* registry
		std::unique_ptr<AssetStore> assetStore;

	public:
		Game();
		~Game();
		void Initialize();
		void Run();
		void LoadLevel(int level);
		void Setup();
		void ProcessInput();
		void Update();
		void Render();
		void Destroy();

		int windowWidth;
		int windowHeight;
};
#endif 