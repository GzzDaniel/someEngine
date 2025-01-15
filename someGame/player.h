#ifndef PLAYER_H_
#define PLAYER_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "controllermanager.h"
#include "gameObject.h"

// TODO use state design pattern 
class Player : public GameObject, public Collider
{
public:

	Player(int posx, int posy, int scale) :
		GameObject(posx, posy),
		Collider(posx + 17 * scale / 2, posy + 23 * scale / 2, 30, 53, TYPE_PLAYER),
		texture(NULL),

		direction(DOWN),
		frameNum(0),
		state(STATE_IDLE),

		verticalVelocity(0),
		HorizontalVelocity(0),
		diagonalFactor(1),

		speed(0.25),
		scale(scale),
		animationDelay(3)
	{
	}
	~Player() {}

	void loadmedia(SDL_Renderer* _renderer);
	void render(SDL_Renderer* _renderer) override;
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

};


#endif /* PLAYER_H_ */