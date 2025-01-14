#ifndef PLAYER_H_
#define PLAYER_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include "controllermanager.h"

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
	void virtual update() = 0;
	void virtual render() {}
	void virtual onNotify(Event _event) {};

	double virtual getxPos();
	double virtual getyPos();

private:
	double xpos;
	double ypos;
};






class Player : public GameObject
{
public:

	Player(int posx, int posy) : GameObject(posx, posy),

		texture(NULL),

		direction(DOWN),
		frameNum(0),
		state(STATE_IDLE),

		verticalVelocity(0),
		HorizontalVelocity(0),
		diagonalFactor(1),

		speed(0.25),
		scale(3),
		animationDelay(3)
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

};


#endif /* PLAYER_H_ */