#include "Game.h"
#include "../Logger/Logger.h"
#include "./LevelLoader.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderHealthBarSystem.h"
#include "../Systems/RenderGUISystem.h"
#include "../Systems/ScriptSystem.h"
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h> // render SDL2 (ImGui SDL helper)
#include <imgui/imgui_impl_sdl.h> // integrate ImGui with SDL

// This is not apply ECS architecture
//glm::vec2 playerPosition;
//glm::vec2 playerVelocity;

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() {
	isRunning = false;
	isDebug = false;
	registry = std::make_unique<Registry>();
	assetStore = std::make_unique<AssetStore>();
	eventBus = std::make_unique<EventBus>();

	Logger::Log("Game constructor called!");
}

Game::~Game() {
	Logger::Log("Game destructor called!");
}

void Game::Initialize() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Logger::Err("Error Initializing SDL");
		return;
	}

	if (TTF_Init() != 0) {
		Logger::Err("Error Initializing SDL TTF");
		return;
	}
	// TODO: create a windows
	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	windowWidth = displayMode.w;
	windowHeight = displayMode.h;
	window = SDL_CreateWindow(
		NULL, 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		windowWidth,
		windowHeight,
		SDL_WINDOW_BORDERLESS
	);

	if (!window) {
		Logger::Err("Error creating SDL window.");
		return;
	}
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		Logger::Err("Error creating SDL renderer");
		return;
	}

	// Initialize the imgui context
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, windowWidth, windowHeight);

	// Initialize the camera view with the entire screen area
	camera.x = 0;
	camera.y = 0;
	camera.w = windowWidth;
	camera.h = windowHeight;

	isRunning = true;
}

void Game::Run() {
	Setup();
	while (isRunning) {
		ProcessInput();
		Update();
		Render();
	}
}

void Game::ProcessInput() {
	SDL_Event sdlEvent;
	while (SDL_PollEvent(&sdlEvent)) {
		// ImGui SDL input
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		ImGuiIO& io = ImGui::GetIO();

		int mouseX, mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

		io.MousePos = ImVec2(mouseX, mouseY);
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT); // BIT WISE
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT); // BIT WISE

		// Handle core SDL events (close window, key pressed, ...)
		switch (sdlEvent.type)
		{
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
					isRunning = false;
				}
				if (sdlEvent.key.keysym.sym == SDLK_d) {
					isDebug = !isDebug;
				}
				eventBus->EmitEvent<KeyPressedEvent>(sdlEvent.key.keysym.sym);
				break;
			default:
				break;
		}
	
	};
}

void Game::Setup() {
	// Add the system that need to be processed in our game
	registry->AddSystem<MovementSystem>();
	registry->AddSystem<RenderSystem>();
	registry->AddSystem<AnimationSystem>();
	registry->AddSystem<CollisionSystem>();
	registry->AddSystem<RenderColliderSystem>();
	registry->AddSystem<DamageSystem>();
	registry->AddSystem<KeyboardControlSystem>();
	registry->AddSystem<CameraMovementSystem>();
	registry->AddSystem<ProjectileEmitSystem>();
	registry->AddSystem<ProjectileLifecycleSystem>();
	registry->AddSystem<RenderTextSystem>();
	registry->AddSystem<RenderHealthBarSystem>();
	registry->AddSystem<RenderGUISystem>();
	registry->AddSystem<ScriptSystem>();

	// Create the bindings between C++ and Lua
	registry->GetSystem<ScriptSystem>().CreateLuaBindings(lua);

	// Load the first Level
	LevelLoader loader;
	lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);
	loader.LoadLevel(lua, registry, assetStore, renderer, 1);
}

void Game::Update() {
	// TODO: If we are too fast, waste some time until we reach the MILLISECS_PER_FRAME
	// while (!SDL_TICKS_PASSED(SDL_GetTicks(), millisecsPreviousFrame + MILLISECS_PER_FRAME)); // not incredibly smart => not frendly with resource
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
		SDL_Delay(timeToWait);
	}

	// The difference in ticks since the last frame, converted to seconds
	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

	// store the current frame time
	millisecsPreviousFrame = SDL_GetTicks();

	// TODO: Update game objects 
	// Not apply ECS architecture
	/*playerPosition.x += playerVelocity.x * deltaTime;
	playerPosition.y += playerVelocity.y * deltaTime;*/

	// apply ECS architecture
	// TODO:
	// MovementSystem.Update();
	// CollisionSystem.Update();
	// DamageSystem.Update();
	
	// Reset all event handlers for the current frame
	eventBus->Reset();

	// Perform the subscription of the events for all systems
	registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
	registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);

	// Update the registry to process the entities that are waiting to be created/deleted
	registry->Update();

	// Invoke all the systems that need to update
	registry->GetSystem<MovementSystem>().Update(deltaTime);
	registry->GetSystem<AnimationSystem>().Update();
	registry->GetSystem<CollisionSystem>().Update(eventBus);
	registry->GetSystem<ProjectileEmitSystem>().Update(registry);
	registry->GetSystem<CameraMovementSystem>().Update(camera);
	registry->GetSystem<ProjectileLifecycleSystem>().Update();
	registry->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// Draw a rectangle
	/*SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect player = { 10, 10, 20, 20 };
	SDL_RenderFillRect(renderer, &player);*/

	// Not apply ECS architecture
	// SDL_image.h
	// Load a PNG textture 
	//SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
	//SDL_Texture* textture = SDL_CreateTextureFromSurface(renderer, surface);
	//SDL_FreeSurface(surface);

	//// What is the destination rectangle that we want to place our textture
	//SDL_Rect dstRect = { 
	//	static_cast<int>(playerPosition.x), 
	//	static_cast<int>(playerPosition.y),
	//	32, 
	//	32 
	//};
	//SDL_RenderCopy(renderer, textture, NULL, &dstRect);
	//SDL_DestroyTexture(textture);


	// apply ECS architecture
	// TODO: Render game objects...
	// Invoke all the systems that need to update
	registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
	registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
	registry->GetSystem<RenderHealthBarSystem>().Update(renderer, assetStore, camera);
	if (isDebug) {
		registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);
		
		registry->GetSystem<RenderGUISystem>().Update(registry, camera);
	}

	// double buffer renderer (front and back)
	SDL_RenderPresent(renderer);
}

void Game::Destroy() {
	ImGuiSDL::Deinitialize();
	ImGui::DestroyContext();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}