#ifndef MAINLOOP_H_
#define MAINLOOP_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>

#include "gameObject.h"
#include "controllermanager.h"
#include "player.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* _window = NULL;
SDL_Renderer* _renderer = NULL;

bool running = true;
SDL_Event sdl_event;


Uint32 deltaTime=0 , oldTime=0, accumulator=0;


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


class Obstacle : public GameObject, public Collider
{
public:
	Obstacle(int x, int y):
		GameObject(x, y), 
		Collider(x, y, 100, 100, TYPE_WALL) {}
	~Obstacle() {}
	void update() override {drawCollisionBox(_renderer); }
};


class Subject
{
protected:
	GameObject* inputobserverArray[5] = { 0 };
	GameObject* observerArray[5] = { 0 };
	Collider* collidersArray[5] = { 0 };
	int numInputObservers;
	int numObservers;
	int numColliderObservers;

public:
	Subject() : numInputObservers(0), numObservers(0), numColliderObservers(0) { }
	virtual ~Subject() {}

	void addObserver(GameObject* _observer) {
		observerArray[numObservers] = _observer;
		numObservers++;
	}
	void addInputObserver(GameObject* _observer) {
		inputobserverArray[numInputObservers] = _observer;
		numInputObservers++;
		addObserver(_observer);
	}
	void addColliderObserver(Collider* _observer) {
		collidersArray[numColliderObservers] = _observer;
		numColliderObservers++;
	}
};

class Engine : public Subject
{
public:
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

			while (accumulator > 1.0 / 61.0)
			{	
				update();
				accumulator -= (1.0 / 59.0);
				if (accumulator < 0) accumulator = 0;
			}
			display();
		}
	}
	
	ControllerManager _controllerManager;

private:
	
	void handleInput(ControllerManager* CMP) {
		for (int i = 0; i < numInputObservers; i++) {
			inputobserverArray[i]->handleInput(CMP);
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
					std::cout << "up" << std::endl;
					_controllerManager.pressDpadKey(KEY_PRESS_UP); handleInput(&_controllerManager);
					break;
				case SDLK_DOWN: 
				case SDLK_s:
					std::cout << "down" << std::endl;
					_controllerManager.pressDpadKey(KEY_PRESS_DOWN); handleInput(&_controllerManager);
					break;
				case SDLK_a:
				case SDLK_LEFT: 
					std::cout << "left" << std::endl; 
					_controllerManager.pressDpadKey(KEY_PRESS_LEFT); handleInput(&_controllerManager);
					break;
				case SDLK_d:
				case SDLK_RIGHT: 
					std::cout << "right" << std::endl; 
					_controllerManager.pressDpadKey(KEY_PRESS_RIGHT); handleInput(&_controllerManager);
					break;
				case SDLK_LSHIFT:
					std::cout << "shift" << std::endl;
					_controllerManager.setLastKeyEvent(KEY_PRESS_SHIFT);
					handleInput(&_controllerManager);
					_controllerManager.setLastKeyEvent(KEY_PRESS_NULL);
				}
			}
			else if (sdl_event.type == SDL_KEYUP) {
				switch (sdl_event.key.keysym.sym)
				{
				case SDLK_UP:
				case SDLK_w:
					_controllerManager.releaseDpadKey(KEY_PRESS_UP); handleInput(&_controllerManager);
					std::cout << "up released" << std::endl;
					break;
				case SDLK_DOWN:
				case SDLK_s:
					std::cout << "down released" << std::endl;
					_controllerManager.releaseDpadKey(KEY_PRESS_DOWN); handleInput(&_controllerManager);
					break;
				case SDLK_LEFT:
				case SDLK_a:
					std::cout << "left released" << std::endl;
					_controllerManager.releaseDpadKey(KEY_PRESS_LEFT); handleInput(&_controllerManager);
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					std::cout << "right released" << std::endl;
					_controllerManager.releaseDpadKey(KEY_PRESS_RIGHT); handleInput(&_controllerManager);
					break;
				}
			}	
		}
		// notifies all observers to read all current inputs
		handleInput(&_controllerManager);
		return;
	}
	void update() 
	{		
		for (int i = 0; i < numObservers; i++) 
		{
			observerArray[i]->update();
		}

		// TODO use a smarter implementation for checking collisions
		for (int i = 0; i < numColliderObservers; i++)
		{
			for (int j = 0; j < numColliderObservers; j++) {
				if (i != j) {
					if (collidersArray[i]->isColliding(collidersArray[j])) {
						collidersArray[i]->onCollision(collidersArray[j]);
					}
				}
			}
		}
		return;
	}
	void display() 
	{
		// renders all gameObjects and then switches buffers
		for (int i = 0; i < numObservers; i++) {

			observerArray[i]->render(_renderer);
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