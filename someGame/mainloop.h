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
	KEY_PRESS_NULL,

	KEY_PRESS_UP,
	KEY_PRESS_RIGHT, // 2
	KEY_PRESS_DOWN,
	KEY_PRESS_LEFT, // 4, horizontal presses are even numbers 
	
	NUM_KEY_STATES
};
Keypress _keyPress;

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


// TODO keypress history
// ControllerManager class offers methods to help other class with managinf inputs
class ControllerManager
{
public:
	ControllerManager() : numKeysPressed(0){}
	~ControllerManager() {}
	
	void pressKey(Keypress k);
	void releaseKey(Keypress k);

	// returns the nth currently pressed key
	Keypress getnKeyPressed(int n);

	//returns last keypress
	Keypress getLastKeypress();

	//returns second to last keypress
	Keypress getSecondLastKeypress();

	// returns the chronological index at which the specified keypress arrived
	int getArrivalIndex(Keypress k);

	//returns true if the keypress given was pressed
	bool isKeyPressed(Keypress k);

	//prints the deque
	void showDeque();

	// Gets the KEypress taking into accound physical limitantions of a Dpad
	Keypress getHorizontalDpress();
	Keypress getVerticalDpress();
	Keypress getFirstDpress();
	Keypress getSecondDpress();

private:
	// array of boolean values for the keypresses
	bool KeysPressed[NUM_KEY_STATES] = { false };

	// use to keep track of the index
	int numKeysPressed;
	// array that shoes the order at which the keys were pressed
	Keypress keypressDeque[NUM_KEY_STATES] = { KEY_PRESS_NULL };

};

// GameObject class, some people call it Entity (I mighht change the name at some point). 
// everything on screen is a GameObject
class GameObject
{
public:
	GameObject() : xpos(0), ypos(0) {}
	GameObject(double xpos, double ypos) : xpos(xpos), ypos(ypos) {}
	virtual ~GameObject() {}

	void virtual setPos(double inxpos, double inypos);
	void virtual move(double inxmove, double inymove);

	void virtual handleInput(ControllerManager* CM) {}
	void virtual update()=0; 
	void virtual render() {}
	void virtual onNotify(Event _event) {};

	double virtual getxPos();
	double virtual getyPos();

private:
	double xpos;
	double ypos;
};


class Subject
{
protected:
	GameObject* inputobserverArray[5] = { 0 };
	GameObject* observerArray[5] = { 0 };
	int numInputObservers;
	int numObservers;

public:
	Subject() : numInputObservers(0), numObservers(0) { }
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
	
};


class Player : public GameObject
{
public:

	Player(int posx, int posy) : GameObject(posx, posy),

		texture(NULL),

		direction(DOWN),
		frameNum(0),
		state(IDLE),

		verticalVelocity(0),
		HorizontalVelocity(0),
		diagonalFactor(1),

		speed(0),
		scale(3),
		animationDelay(4)
	{
		loadmedia();
	}
	~Player() {}

	void loadmedia();
	void render() override;
	void update() override;
	//void onNotify(Event _event) override;

	void handleInput(ControllerManager* CM) override;

private:
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
		JUMPING,

		NUMBER_OF_STATES
	};

	SDL_Texture* texture;

	// animation dimentions
	SDL_Rect standingSprites[NUMBER_OF_DIRECTIONS];
	SDL_Rect walkingDownSprites[10];
	SDL_Rect walkingLeftSprites[10];
	SDL_Rect walkingUpSprites[10];
	int frameNum;

	PlayerState state;
	PlayerDirection direction;

	// movement variables
	double verticalVelocity;
	double HorizontalVelocity;
	double diagonalFactor;
	bool moveBools[NUMBER_OF_DIRECTIONS] = { false };

	// constants
	double const speed;
	int const scale;
	int const animationDelay;

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
				//&& _event.key.repeat == 0
				switch (sdl_event.key.keysym.sym )
				{
				case SDLK_UP: 
					/*_keyPress = KEY_PRESS_UP;
					isKeyPressed[KEY_PRESS_UP] = true;*/
					std::cout << "up" << std::endl;
					_controllerManager.pressKey(KEY_PRESS_UP);
					break;
				case SDLK_DOWN: 
					std::cout << "down" << std::endl;
					_controllerManager.pressKey(KEY_PRESS_DOWN);
					break;
				case SDLK_LEFT: 
					std::cout << "left" << std::endl; 
					_controllerManager.pressKey(KEY_PRESS_LEFT);
					break;
				case SDLK_RIGHT: 
					std::cout << "right" << std::endl; 
					_controllerManager.pressKey(KEY_PRESS_RIGHT);
					break;
				}
			}
			else if (sdl_event.type == SDL_KEYUP) {
				switch (sdl_event.key.keysym.sym)
				{
				case SDLK_UP:
					_controllerManager.releaseKey(KEY_PRESS_UP);
					std::cout << "up released" << std::endl;
					break;
				case SDLK_DOWN:
					std::cout << "down released" << std::endl;
					_controllerManager.releaseKey(KEY_PRESS_DOWN);
					break;
				case SDLK_LEFT:
					std::cout << "left released" << std::endl;
					_controllerManager.releaseKey(KEY_PRESS_LEFT);
					break;
				case SDLK_RIGHT:
					std::cout << "right released" << std::endl;
					_controllerManager.releaseKey(KEY_PRESS_RIGHT);
					break;
				}
			}
			/*else {
				std::cout << "no key pressed" << std::endl;
				_keyPress = NO_KEY_PRESSED;
			}*/
			// TODO FIX KEYS IMPLEMENTATION
			
		}
		//std::cout << _controllerManager.getLastKeypress() << _controllerManager.getSecondLastKeypress() << std::endl;

		// notifies all observers to read all current inputs
		handleInput(&_controllerManager);
	
		//_controllerManager.showDeque();

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
		// renders all gameObjects and then switches buffers
		for (int i = 0; i < numObservers; i++) {

			observerArray[i]->render();
		}
		//Update screen
		SDL_RenderPresent(_renderer);
		//Clear screen
		SDL_RenderClear(_renderer);

		// TODO render only characters on screen
	}

};

#endif /* MAINLOOP_H_ */