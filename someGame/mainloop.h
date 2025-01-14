#ifndef MAINLOOP_H_
#define MAINLOOP_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>

#include "controllermanager.h"
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

enum CollisionType {
	TYPE_PLAYER,
	TYPE_WALL,
	TYPE_ENEMY,

	// types used to detect collision direction
	TYPE_HORIZONTAL,
	TYPE_VERTICAL,
	TYPE_TOTAL,
	TYPE_NONE
};

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




// GameObject class, some people call it Entity (I mighht change the name at some point). 
// everything on screen is a GameObject
class GameObject
{
public:
	GameObject() : xpos(0), ypos(0) {}
	GameObject(double xpos, double ypos) : xpos(xpos), ypos(ypos) {}
	virtual ~GameObject() {}

	void virtual setxPos(double inxpos);
	void virtual setyPos(double inypos);
	void virtual move(double inxmove, double inymove);

	void virtual handleInput(ControllerManager* CM) {}
	void virtual update() = 0;
	void virtual render() {}
	void virtual onNotify(Event _event) {};

	double virtual getxPos();
	double virtual getyPos();

private:
	double xpos;
	double ypos;
};

class Collider
{
public:
	Collider(int centerx, int centery, int w, int h, CollisionType t):
		type(t),
		centerx(centerx),
		centery(centery),
		halfWidth(w/2),
		halfHeight(h/2),
		prevCenterx(centerx),
		prevCentery(centery)
		{}
	~Collider() {}

	// returns true if the object is colliding with the specified object
	bool isColliding(Collider* c);

	// draws the hitbox using 4 lines
	void drawCollisionBox();

	// places the center at specified position
	void setColliderCenter(int x, int y);

	// returns the collider type
	CollisionType getType();

	// returns the previous frame collision for direction detection
	CollisionType getPrevCollision(Collider* c);

	// handle collision with other colliders
	void virtual onCollision(Collider* c) {}


	int getHalfWidth() { return halfWidth; }
	int getHalfHeight() { return halfHeight; }
	int getCenterx() { return centerx; }
	int getCentery() { return centery; }

private:
	bool isverticalColliding(Collider* c);
	bool isHorizontalColliding(Collider* c);

	CollisionType type;

	int centerx;
	int centery;
	int halfWidth;
	int halfHeight;

	int prevCenterx;
	int prevCentery;
};

// TODO use state design pattern 
class Player : public GameObject, public Collider
{
public:

	Player(int posx, int posy, int scale) : 
		GameObject(posx, posy), 
		Collider(posx+17*scale/2, posy+23*scale/2, 30, 53, TYPE_PLAYER),
		texture(NULL),

		direction(DOWN),
		frameNum(0),
		state(STATE_IDLE),

		verticalVelocity(0),
		HorizontalVelocity(0),
		diagonalFactor(1),

		speed(0.25),
		scale(scale),
		animationDelay(3),
		inMeowZone(false)
	{
		loadmedia();
	}
	~Player() {}

	void loadmedia();
	void render() override;
	void update() override;
	void onCollision(Collider* other) override;
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
		STATE_WALKING,
		STATE_IDLE,
		STATE_JUMPING,

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

	// other bools
	bool inMeowZone;

};

class Obstacle : public GameObject, public Collider
{
public:
	Obstacle(int x, int y):
		GameObject(x, y), 
		Collider(x, y, 100, 100, TYPE_WALL) {}
	~Obstacle() {}
	void update() override {drawCollisionBox(); }
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
					_controllerManager.pressKey(KEY_PRESS_UP);
					break;
				case SDLK_DOWN: 
				case SDLK_s:
					std::cout << "down" << std::endl;
					_controllerManager.pressKey(KEY_PRESS_DOWN);
					break;
				case SDLK_a:
				case SDLK_LEFT: 
					std::cout << "left" << std::endl; 
					_controllerManager.pressKey(KEY_PRESS_LEFT);
					break;
				case SDLK_d:
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
				case SDLK_w:
					_controllerManager.releaseKey(KEY_PRESS_UP);
					std::cout << "up released" << std::endl;
					break;
				case SDLK_DOWN:
				case SDLK_s:
					std::cout << "down released" << std::endl;
					_controllerManager.releaseKey(KEY_PRESS_DOWN);
					break;
				case SDLK_LEFT:
				case SDLK_a:
					std::cout << "left released" << std::endl;
					_controllerManager.releaseKey(KEY_PRESS_LEFT);
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					std::cout << "right released" << std::endl;
					_controllerManager.releaseKey(KEY_PRESS_RIGHT);
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

			observerArray[i]->render();
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