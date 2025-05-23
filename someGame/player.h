#ifndef PLAYER_H_
#define PLAYER_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "controllermanager.h"
#include "gameObject.h"

//0.7071067811865476
double const DIAGONAL_FACTOR = 0.7071;

enum PlayerDirection {
	DOWN,
	LEFT,
	UP,
	RIGHT,
	NUMBER_OF_DIRECTIONS
};

enum PlayerStateID {
	IDLE,
	WALKING,
	ROLLING,
	JUMPING,
	ATTACKING
};

// forward declare for the state classes
class Player;

class PlayerState
{
public:
	virtual ~PlayerState() {}
	virtual void handleInput(Player* player, ControllerManager* controller) {}
	virtual void update(Player* player) {}
	virtual void render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera) {}
	virtual PlayerStateID getStateID() = 0;
	virtual std::string getName() = 0;
	virtual void initialize(Player* player) {};
protected:
	void changeState(Player* player, PlayerState* state);
};

class IdleState : public PlayerState
{
public:
	~IdleState() {}
	void handleInput(Player* player, ControllerManager* controller) override;
	void update(Player* player) override;
	void render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera) override;
	static PlayerState* instance() {
		static IdleState inst;
		return &inst;
	}
	PlayerStateID getStateID() override { return IDLE; }
	std::string getName() { return "Idle"; }

private:
	bool shiftPressed;

};

class WalkingState : public PlayerState
{
public:
	~WalkingState() {}
	void handleInput(Player* player, ControllerManager* controller) override;
	void update(Player* player) override;
	void render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera) override;
	static PlayerState* instance() {
		static WalkingState inst;
		return &inst;
	}
	PlayerStateID getStateID() override { return WALKING; }
	std::string getName() { return "walking"; }

};

class RollState : public PlayerState
{
public:
	RollState() : _count(0) {}
	~RollState() {}
	void handleInput(Player* player, ControllerManager* controller) override;
	void update(Player* player) override;
	void render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera) override;
	static PlayerState* instance() {
		static RollState inst;
		return &inst;
	}
	PlayerStateID getStateID() override { return ROLLING; }
	std::string getName() { return "roll"; }
private:
	int _count;
};

class JumpingState : public PlayerState
{
public:
	~JumpingState() {}
	void handleInput(Player* player, ControllerManager* controller) override;
	void update(Player* player) override;
	void render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera) override;
	static PlayerState* instance() {
		static JumpingState inst;
		return &inst;
	}
	PlayerStateID getStateID() override { return JUMPING; }
	std::string getName() { return "jumping"; }

	void initialize(Player* player) override;
private:
	double maxSpeed;
};

class AttackState : public PlayerState
{
public:
	~AttackState() {}
	void handleInput(Player* player, ControllerManager* controller) override;
	void update(Player* player) override;
	void render(Player* player, SDL_Renderer* renderer, SDL_Rect* camera) override;
	static PlayerState* instance() {
		static AttackState inst;
		return &inst;
	}
	PlayerStateID getStateID() override { return ATTACKING; }
	std::string getName() { return "attacking"; }

};


class Player : public GameObject, public ColliderManager, public SpriteRenderer
{
public:

	Player(int posx, int posy, int scale) :
		GameObject(posx, posy),
		SpriteRenderer(posx, posy, 32, 32, scale, -16, -27),
		scale(scale),

		direction(DOWN),
		frameNum(0),

		verticalVelocity(0),
		HorizontalVelocity(0),
		diagonalFactor(1),

		speed(0.18),
		animationDelay(3),

		_state(IdleState::instance())
	{
		Collider col(posx, posy, 30, 30, TYPE_PLAYER, 0, -10);
		addNewCollider(IDLE ,col);

	}
	~Player() { }

	
	void render(SDL_Renderer* _renderer, SDL_Rect* camera) override;

	void update() override;
	void onCollision(Collider* pc1, Collider* pc2) override;
	//void onNotify(Event _event) override;

	void handleInput(ControllerManager* CM) override;

	void defineSrcSprites() override;

	void changeState(PlayerState* state) {
		_state = state;
	}
	PlayerState* getState() {
		return _state;
	}

private:
	friend class WalkingState;
	friend class IdleState;
	friend class RollState;
	friend class PlayerState;
	friend class JumpingState;

	int scale;

	// animation dimentions
	SDL_Rect standingSprites[NUMBER_OF_DIRECTIONS];
	SDL_Rect walkingDownSprites[10];
	SDL_Rect walkingLeftSprites[10];
	SDL_Rect walkingUpSprites[10];
	SDL_Rect rollingDownSprites[10];
	SDL_Rect rollingLeftSprites[10];
	SDL_Rect rollingUpSprites[10];
	SDL_Rect shadowSprite;
	int frameNum;

	PlayerDirection direction;

	// movement variables
	double verticalVelocity;
	double HorizontalVelocity;
	double diagonalFactor;
	bool moveBools[NUMBER_OF_DIRECTIONS] = { false };

	// constants
	double speed;
	//int const scale;
	int const animationDelay;

	// states
	PlayerState* _state;

};






#endif /* PLAYER_H_ */