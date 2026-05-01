#ifndef MAINLOOP_H_
#define MAINLOOP_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>

#include "gameObject.h"
#include "controllermanager.h"
#include "player.h"
#include "SecondaryEntities.h"

const int LEVEL_WIDTH = 740;
const int LEVEL_HEIGHT = 600;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* _window = NULL;
SDL_Renderer* _renderer = NULL;

bool running = true;
SDL_Event sdl_event;


// TODO [Claude] BUG: accumulator should be a double (or float), not Uint32.
// SDL_GetTicks() returns milliseconds as Uint32, so deltaTime is fine as Uint32,
// but accumulator is compared against 1.0/61.0 (~0.016) which truncates to 0 when
// stored back into a Uint32 — the subtraction never decreases accumulator, causing
// an infinite loop inside run(). Fix: declare accumulator (and ideally deltaTime) as double,
// and convert SDL_GetTicks() results to seconds (divide by 1000.0) before accumulating.
Uint32 deltaTime=0 , oldTime=0, accumulator=0;

// TODO [Claude] BUG: camera width should be SCREEN_WIDTH (640), not SCREEN_HEIGHT (480).
// As-is the camera rect is 480x480 instead of 640x480, so the right edge of the screen
// is never included in Camera.w bounds checks in display().
SDL_Rect Camera = {0, 0, SCREEN_HEIGHT, SCREEN_HEIGHT};

bool initializeSDL()
{
	// Initiate SDL and handle errors if the init funcion returns negative
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "Failed initializing SDL " << SDL_GetError() << "\n";
		return false;
	}

	_window = SDL_CreateWindow("SomeGame", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (_window == NULL) {
		std::cout << "Failed creating window " << SDL_GetError() << "\n";
		return false;
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (_renderer == NULL) {
		std::cout << "Failed creating renderer " << SDL_GetError() << "\n";
		return false;
	}

	SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		std::cout << "Failed creating SDLIMG " << IMG_GetError() << "\n";
		return false;
	};
	
	return true;
}
void close() {
	SDL_DestroyRenderer(_renderer);
	SDL_DestroyWindow(_window);

	_window = NULL;
	_renderer = NULL;

	IMG_Quit();
	SDL_Quit();
}


// [Claude] Subject is the engine's observer registry.
// Four separate arrays because not every object needs all four roles:
//   observerArray      — receives update() every frame
//   inputobserverArray — receives handleInput() on key events (also added to observerArray via addInputObserver)
//   collidersArray     — participates in collision detection pairs each frame
//   rendersArray       — rendered each frame, Y-sorted before drawing
class Subject
{
protected:
	std::vector<GameObject*> inputobserverArray;
	std::vector<GameObject*> observerArray;
	std::vector<ColliderManager*> collidersArray;
	std::vector<SpriteRenderer*> rendersArray;
	/*int numRenderedObservers;
	int numInputObservers;
	int numObservers;
	int numColliderObservers;*/

public:
	//Subject() : numRenderedObservers(0), numInputObservers(0), numObservers(0), numColliderObservers(0)  { }
	virtual ~Subject() {}

	void addObserver(GameObject* _observer) {
		//observerArray[numObservers] = _observer;
		//numObservers++;
		observerArray.push_back(_observer);
	}
	void addRenderedObserver(SpriteRenderer* _observer) {
		//rendersArray[numRenderedObservers] = _observer;
		//numRenderedObservers++;
		rendersArray.push_back(_observer);
	}
	void addInputObserver(GameObject* _observer) {
		//inputobserverArray[numInputObservers] = _observer;
		//numInputObservers++;
		addObserver(_observer);
		inputobserverArray.push_back(_observer);
	}
	void addColliderObserver(ColliderManager* _observer) {
		//collidersArray[numColliderObservers] = _observer;
		//numColliderObservers++;
		collidersArray.push_back(_observer);
	}
};

class Engine : public Subject
{
public:
	void addCameraTarget(GameObject* target) {
		cameraTarget = target;
	}

	Engine() {
		if (!initializeSDL()) {
			std::cout << "error initializing" << std::endl;
		}
	}
	~Engine() { close(); }

	void run()
	{
		while (running)
		{
			input();

			deltaTime = SDL_GetTicks() - oldTime;
			oldTime = SDL_GetTicks();
			accumulator += deltaTime;

			// TODO [Claude] BUG: two mismatches here —
		// 1) The threshold (1.0/61.0) and subtraction (1.0/59.0) use different frame rates.
		//    Both should use the same target rate, e.g. both 1.0/60.0.
		// 2) Because accumulator is Uint32 (see declaration above), the floating-point
		//    subtraction result truncates to 0 and accumulator never drains — infinite loop.
		while (accumulator > 1.0 / 61.0)
			{
				update();
				accumulator = accumulator - (1.0 / 59.0);
				if (accumulator < 0) accumulator = 0;
			}
			display();
			
		}
	}
	
	ControllerManager _controllerManager;

private:
	GameObject* cameraTarget;
	
	void handleInput(ControllerManager* CMP) {
		/*for (int i = 0; i < numInputObservers; i++) {
			inputobserverArray[i]->handleInput(CMP);
		}*/
		for (auto& inputObserver : inputobserverArray) {
			inputObserver->handleInput(CMP);
		}
	}

	// Loop Methods
	void input()
	{
		while (SDL_PollEvent(&sdl_event) != 0) {
			if (sdl_event.type == SDL_QUIT) {
				running = false;
			}
			else if (sdl_event.type == SDL_KEYDOWN && sdl_event.key.repeat == 0)
			{
				switch (sdl_event.key.keysym.sym )
				{
				case SDLK_UP: 
				case SDLK_w:
					//std::cout << "up" << std::endl;
					_controllerManager.pressDpadKey(KEY_PRESS_UP); handleInput(&_controllerManager);
					break;
				case SDLK_DOWN: 
				case SDLK_s:
					//std::cout << "down" << std::endl;
					_controllerManager.pressDpadKey(KEY_PRESS_DOWN); handleInput(&_controllerManager);
					break;
				case SDLK_a:
				case SDLK_LEFT: 
					//std::cout << "left" << std::endl; 
					_controllerManager.pressDpadKey(KEY_PRESS_LEFT); handleInput(&_controllerManager);
					break;
				case SDLK_d:
				case SDLK_RIGHT: 
					//std::cout << "right" << std::endl; 
					_controllerManager.pressDpadKey(KEY_PRESS_RIGHT); handleInput(&_controllerManager);
					break;
				case SDLK_LSHIFT:
					//std::cout << "shift" << std::endl;
					_controllerManager.setLastKeyEvent(KEY_PRESS_SHIFT);
					handleInput(&_controllerManager);
					//_controllerManager.setLastKeyEvent(KEY_PRESS_NULL);
					break;
				case SDLK_SPACE:
					//std::cout << "space pressed" << std::endl;
					_controllerManager.setLastKeyEvent(KEY_PRESS_SPACE);
					handleInput(&_controllerManager);
					break;
				}
			}
			else if (sdl_event.type == SDL_KEYUP) {
				switch (sdl_event.key.keysym.sym)
				{
				case SDLK_UP:
				case SDLK_w:
					_controllerManager.releaseDpadKey(KEY_PRESS_UP); handleInput(&_controllerManager);
					//std::cout << "up released" << std::endl;
					break;
				case SDLK_DOWN:
				case SDLK_s:
					//std::cout << "down released" << std::endl;
					_controllerManager.releaseDpadKey(KEY_PRESS_DOWN); handleInput(&_controllerManager);
					break;
				case SDLK_LEFT:
				case SDLK_a:
					//std::cout << "left released" << std::endl;
					_controllerManager.releaseDpadKey(KEY_PRESS_LEFT); handleInput(&_controllerManager);
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					//std::cout << "right released" << std::endl;
					_controllerManager.releaseDpadKey(KEY_PRESS_RIGHT); handleInput(&_controllerManager);
					break;
				case SDLK_LSHIFT:
					//std::cout << "shift released" << std::endl;
					_controllerManager.setLastKeyEvent(KEY_RELE_SHIFT);
					handleInput(&_controllerManager);
					break;
				case SDLK_SPACE:
					//std::cout << "space released" << std::endl;
					_controllerManager.setLastKeyEvent(KEY_RELE_SPACE);
					handleInput(&_controllerManager);
					break;
				}
			}	
		} //  end of event queue

		_controllerManager.setLastKeyEvent(KEY_PRESS_NULL);

		// notifies all observers to read all current inputs
		handleInput(&_controllerManager);
		return;
	}
	void update() 
	{		
		//for (int i = 0; i < numObservers; i++) 
		//{
		//	observerArray[i]->update();
		//}
		for (auto& observer : observerArray) {
			observer->update();
		}

		// TODO use a faster implementation for checking collisions
		// [Claude] current approach is O(n^2) pairs. The i+1 start avoids checking the same pair twice
		// (A vs B and B vs A), which is why areColliding() only needs to be called once per pair.
		// Note: the `if (i != j)` check is always true because j starts at i+1, so it can be removed.
		int numColliderObservers = collidersArray.size();
		for (int i = 0; i < numColliderObservers; i++)
		{
			for (int j = i+1; j < numColliderObservers; j++) {
				if (i != j) {
					/*if (collidersArray[i]->areColliding(collidersArray[j])) {
						collidersArray[i]->onCollision(collidersArray[j]);
					}*/
					
					collidersArray[i]->areColliding(collidersArray[j]);
				}
			}
		}
		return;
	}
	void display() 
	{
		Camera.x = cameraTarget->getxPos() - SCREEN_WIDTH/2;
		Camera.y = cameraTarget->getyPos() - SCREEN_HEIGHT/2;


		//Keep the camera in bounds
		if (Camera.x < 0)
		{
			Camera.x = 0;
		}
		if (Camera.y < 0)
		{
			Camera.y = 0;
		}
		if (Camera.x > LEVEL_WIDTH - Camera.w)
		{
			Camera.x = LEVEL_WIDTH - Camera.w;
		}
		if (Camera.y > LEVEL_HEIGHT - Camera.h)
		{
			Camera.y = LEVEL_HEIGHT - Camera.h;
		}


		// renders all gameObjects 
		// TODO Ysort camera
		std::sort(rendersArray.begin(), rendersArray.end(), [](auto* a, auto* b) {
			return a->getYcamValue() < b->getYcamValue();
		});

		//for (int i = 0; i < numRenderedObservers; i++) {
		//	rendersArray[i]->render(_renderer, &Camera);
		//}
		for (auto& renderObserver : rendersArray) {
			
			renderObserver->render(_renderer, &Camera);
		}

		//Update screen
		SDL_RenderPresent(_renderer);
		//Clear screen
		SDL_SetRenderDrawColor(_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(_renderer);

		// TODO render only characters on screen
	}

};

#endif /* MAINLOOP_H_ */