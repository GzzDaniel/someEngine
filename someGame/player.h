#ifndef PLAYER_H_
#define PLAYER_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "controllermanager.h"
#include "gameObject.h"

enum PlayerDirection {
	DOWN,
	LEFT,
	UP,
	RIGHT,
	NUMBER_OF_DIRECTIONS
};

// forward declare for the state classes
class Player;

class PlayerState
{
public:
	virtual ~PlayerState() {}
	virtual void handleInput(Player* player, ControllerManager* controller) {}
	virtual void update(Player* player) {}
	virtual void render(Player* player, SDL_Renderer* renderer) {}
protected:
	void changeState(Player* player, PlayerState* state);
};

class IdleState : public PlayerState
{
public:
	~IdleState() {}
	void handleInput(Player* player, ControllerManager* controller) override;
	void update(Player* player) override;
	void render(Player* player, SDL_Renderer* renderer) override;
	static PlayerState* instance() {
		static IdleState inst;
		return &inst;
	}

};

class WalkingState : public PlayerState
{
public:
	~WalkingState() {}
	void handleInput(Player* player, ControllerManager* controller) override;
	void update(Player* player) override;
	void render(Player* player, SDL_Renderer* renderer) override;
	static PlayerState* instance() {
		static WalkingState inst;
		return &inst;
	}

};

class RollState : public PlayerState
{
public:
	RollState() : _count(0) {}
	~RollState() {}
	void handleInput(Player* player, ControllerManager* controller) override;
	void update(Player* player) override;
	void render(Player* player, SDL_Renderer* renderer) override;
	static PlayerState* instance() {
		static RollState inst;
		return &inst;
	}
private:
	int _count;
};

class JumpingState : public PlayerState
{
public:
	~JumpingState() {}
	void handleInput(Player* player, ControllerManager* controller) override;
	void update(Player* player) override;
	void render(Player* player, SDL_Renderer* renderer) override;
	static PlayerState* instance() {
		static JumpingState inst;
		return &inst;
	}
};



class Player : public GameObject, public Collider
{
public:

	Player(int posx, int posy, int scale) :
		GameObject(posx, posy),
		Collider(posx + 17 * scale / 2, posy + 23 * scale / 2, 30, 53, TYPE_PLAYER),
		texture(NULL),

		direction(DOWN),
		frameNum(0),

		verticalVelocity(0),
		HorizontalVelocity(0),
		diagonalFactor(1),

		speed(0.25),
		scale(scale),
		animationDelay(3),
		_state(IdleState::instance())
	{
	}
	~Player() {}

	void loadmedia(SDL_Renderer* _renderer);
	void render(SDL_Renderer* _renderer) override;
	void update() override;
	void onCollision(Collider* other) override;
	//void onNotify(Event _event) override;

	void handleInput(ControllerManager* CM) override;

	void changeState(PlayerState* state) {
		_state = state;
	}

private:
	friend class WalkingState;
	friend class IdleState;
	friend class RollState;

	SDL_Texture* texture;

	// animation dimentions
	SDL_Rect standingSprites[NUMBER_OF_DIRECTIONS];
	SDL_Rect walkingDownSprites[10];
	SDL_Rect walkingLeftSprites[10];
	SDL_Rect walkingUpSprites[10];
	SDL_Rect rollingDownSprites[10];
	SDL_Rect rollingLeftSprites[10];
	SDL_Rect rollingUpSprites[10];
	int frameNum;

	PlayerDirection direction;

	// movement variables
	double verticalVelocity;
	double HorizontalVelocity;
	double diagonalFactor;
	bool moveBools[NUMBER_OF_DIRECTIONS] = { false };

	// constants
	double speed;
	int const scale;
	int const animationDelay;

	// states
	PlayerState* _state;

};






#endif /* PLAYER_H_ */