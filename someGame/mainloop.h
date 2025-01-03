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
SDL_Event _event;


enum Event {
	KEY_PRESS_NULL,
	KEY_PRESS_UP,
	KEY_PRESS_DOWN,
	KEY_PRESS_LEFT,
	KEY_PRESS_RIGHT,

	NO_KEY_PRESSED,
	NUM_EVENTS
};

Event keyPress;

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
	GameObject(int xpos, int ypos) : xpos(xpos), ypos(ypos) {}
	virtual ~GameObject() {}

	void virtual setPos(int inxpos, int inypos);
	void virtual move(int inxmove, int inymove);
	void virtual update()=0; 

	int virtual getxPos();
	int virtual getyPos();

private:
	int xpos;
	int ypos;
};

class Observer
{
public:
	void virtual onNotify(Event _event) = 0;
};
class Subject
{
protected:
	Observer* observerArray[5];
	int numObservers;

public:
	Subject() : numObservers(0) {}
	virtual ~Subject() {}

	void addInputObserver(Observer* _observer) {
		observerArray[numObservers] = _observer;
		numObservers++;
	}
};



class Player : public GameObject, public Observer
{
private:
	SDL_Texture* texture;

	enum PlayerSprite {
		FACING_DOWN,
		FACING_LEFT,
		FACING_UP,
		FACING_RIGHT,
		NUMBER_OF_SPRITES
	};
	PlayerSprite prevSprite;
	

public:
	Player() : GameObject(0, 0), texture(NULL), prevSprite(FACING_DOWN)
	{
		loadmedia();
	}
	~Player() {}
	Player(int posx, int posy) : GameObject(posx, posy), texture(NULL), prevSprite(FACING_DOWN) { loadmedia(); }

	void loadmedia();
	void render( PlayerSprite _sprite);
	void update() override;
	void onNotify(Event _event) override;

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
			update();
			display();
		}
	}

private:

	// Loop Methods
	void input()
	{
		while (SDL_PollEvent(&_event) != 0) {
			if (_event.type == SDL_QUIT) {
				running = false;
			}
			if (_event.type == SDL_KEYDOWN)
			{
				switch (_event.key.keysym.sym)
				{
				case SDLK_UP: 
					keyPress = KEY_PRESS_UP;
					std::cout << "up" << std::endl;
					break;
				case SDLK_DOWN: 
					std::cout << "down" << std::endl;
					keyPress = KEY_PRESS_DOWN;
					break;
				case SDLK_LEFT: 
					std::cout << "left" << std::endl; 
					keyPress = KEY_PRESS_LEFT;
					break;
				case SDLK_RIGHT: 
					std::cout << "right" << std::endl; 
					keyPress = KEY_PRESS_RIGHT;
					break;
				}
			}
			if (_event.type == SDL_KEYUP) {
				keyPress = NO_KEY_PRESSED;
			}

			// TODO implement more buttons idk

		}
		
		
		for (int i = 0; i < numObservers; i++) {
			
			observerArray[i]->onNotify(keyPress);
		}

		return;
	}
	void update() {
		
		return;
	}
	void display() 
	{
		//Update screen
		SDL_RenderPresent(_renderer);
		//Clear screen
		SDL_RenderClear(_renderer);
	}

};

#endif /* MAINLOOP_H_ */