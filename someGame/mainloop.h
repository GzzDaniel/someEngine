#ifndef MAINLOOP_H_
#define MAINLOOP_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

//#include "player.h"


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Window* _window = NULL;
SDL_Renderer* _renderer = NULL;

bool running = true;
SDL_Event sdl_event;

enum Event {
	NUM_EVENTS
};
Event _event;


enum Keypress {
	//KEY_PRESS_NULL,
	KEY_PRESS_UP,
	KEY_PRESS_DOWN,
	KEY_PRESS_LEFT,
	KEY_PRESS_RIGHT,

	//NO_KEY_PRESSED,
	NUM_KEYPRESSES
};

Keypress _keyPress;


// array of boolean values for the keypresses
bool isKeyPressed[NUM_KEYPRESSES] = {false};


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

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
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



class GameObject
{
public:
	GameObject() : xpos(0), ypos(0) {}
	GameObject(double xpos, double ypos) : xpos(xpos), ypos(ypos) {}
	virtual ~GameObject() {}

	void virtual setPos(double inxpos, double inypos);
	void virtual move(double inxmove, double inymove);
	void virtual update()=0; 
	void virtual render() {}
	void virtual onNotify(Event _event) {};

	double virtual getxPos();
	double virtual getyPos();

private:
	double xpos;
	double ypos;
};

class ControllerManager
{
public:
	ControllerManager() {}
	~ControllerManager() {}

	void virtual handleInput(Keypress k) = 0;
};

class Subject
{
protected:
	ControllerManager* inputobserverArray[5];
	GameObject* observerArray[5];
	int numInputObservers;
	int numObservers;

public:
	Subject() : numInputObservers(0), numObservers(0) { }
	virtual ~Subject() {}

	void addObserver(GameObject* _observer) {
		observerArray[numObservers] = _observer;
		numObservers++;
	}
	void addInputObserver(ControllerManager* _observer) {
		inputobserverArray[numInputObservers] = _observer;
		numInputObservers++;
	}
	
};



class Player : public GameObject, public ControllerManager
{
private:
	enum PlayerSprite {
		FACING_DOWN,
		FACING_LEFT,
		FACING_UP,
		FACING_RIGHT,
		NUMBER_OF_SPRITES
	};
	enum PlayerDirection {
		DOWN,
		LEFT,
		UP,
		RIGHT,
		NUMBER_OF_DIRECTIONS
	};
	enum PlayerState {
		WALKING,
		IDLE,
		NUMBER_OF_STATES
	};

	SDL_Texture* texture;
	PlayerSprite currSprite;
	SDL_Rect quadsArray[NUMBER_OF_SPRITES];
	PlayerState state;
	PlayerDirection direction;

public:
	Player() : GameObject(0, 0), texture(NULL), currSprite(FACING_DOWN), direction(DOWN), state(IDLE)
	{
		loadmedia();
	}
	~Player() {}
	Player(int posx, int posy) : GameObject(posx, posy), texture(NULL), currSprite(FACING_DOWN) { loadmedia(); }

	void loadmedia();
	void render( ) override;
	void update() override;
	//void onNotify(Event _event) override;
	void handleInput(Keypress k) override;

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

	void gameLoop()
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
	
private:
	void handleInput(Keypress k) {
		for (int i = 0; i < numInputObservers; i++) {

			inputobserverArray[i]->handleInput(k);
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
				//&& _event.key.repeat == 0
				switch (sdl_event.key.keysym.sym )
				{
				case SDLK_UP: 
					_keyPress = KEY_PRESS_UP;
					isKeyPressed[KEY_PRESS_UP] = true;
					std::cout << "up" << std::endl;
					break;
				case SDLK_DOWN: 
					std::cout << "down" << std::endl;
					_keyPress = KEY_PRESS_DOWN;
					isKeyPressed[KEY_PRESS_DOWN] = true;
					break;
				case SDLK_LEFT: 
					std::cout << "left" << std::endl; 
					_keyPress = KEY_PRESS_LEFT; 
					isKeyPressed[KEY_PRESS_LEFT] = true;
					break;
				case SDLK_RIGHT: 
					std::cout << "right" << std::endl; 
					_keyPress = KEY_PRESS_RIGHT;
					isKeyPressed[KEY_PRESS_RIGHT] = true;
					break;
				}
			}
			else if (sdl_event.type == SDL_KEYUP) {
				switch (sdl_event.key.keysym.sym)
				{
				case SDLK_UP:
					//_keyPress = KEY_PRESS_UP;
					isKeyPressed[KEY_PRESS_UP] = false;
					std::cout << "up released" << std::endl;
					break;
				case SDLK_DOWN:
					std::cout << "down released" << std::endl;
					//_keyPress = KEY_PRESS_DOWN;
					isKeyPressed[KEY_PRESS_DOWN] = false;
					break;
				case SDLK_LEFT:
					std::cout << "left released" << std::endl;
					//_keyPress = KEY_PRESS_LEFT; 
					isKeyPressed[KEY_PRESS_LEFT] = false;
					break;
				case SDLK_RIGHT:
					std::cout << "right released" << std::endl;
					//_keyPress = KEY_PRESS_RIGHT;
					isKeyPressed[KEY_PRESS_RIGHT] = false;
					break;
				}
			}
			/*else {
				std::cout << "no key pressed" << std::endl;
				_keyPress = NO_KEY_PRESSED;
			}*/
			// TODO FIX KEYS IMPLEMENTATION
			
		}
		//std::cout << "last keypress: " << _keyPress <<std::endl;
		//handleInput(_keyPress);
	

		

		return;
	}
	void update() 
	{		
		for (int i = 0; i < numObservers; i++) {

			observerArray[i]->update();
		}
		return;
	}
	void display() 
	{
		for (int i = 0; i < numObservers; i++) {

			observerArray[i]->render();
		}
		//Update screen
		SDL_RenderPresent(_renderer);
		//Clear screen
		SDL_RenderClear(_renderer);
	}

};

#endif /* MAINLOOP_H_ */