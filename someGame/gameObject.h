#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>

#include "controllermanager.h"

enum Event {
	NUM_EVENTS
};
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
	void virtual render(SDL_Renderer* _renderer) {}
	void virtual onNotify(Event _event) {};

	double virtual getxPos();
	double virtual getyPos();

private:
	double xpos;
	double ypos;
};

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
class Collider
{
public:
	Collider(int centerx, int centery, int w, int h, CollisionType t) :
		type(t),
		centerx(centerx),
		centery(centery),
		halfWidth(w / 2),
		halfHeight(h / 2),
		prevCenterx(centerx),
		prevCentery(centery)
	{}
	~Collider() {}

	// returns true if the object is colliding with the specified object
	bool isColliding(Collider* c);

	// draws the hitbox using 4 lines
	void drawCollisionBox(SDL_Renderer* _renderer);

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



#endif /*GAMEOBJECT_H_*/