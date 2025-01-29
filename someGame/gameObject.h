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
	
	void virtual onNotify(Event _event) {};

	double virtual getxPos();
	double virtual getyPos();

	void drawGOPoint(SDL_Renderer* renderer, SDL_Rect* camera) {
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
		SDL_RenderDrawPoint(renderer, (int)xpos - camera->x, (int)ypos - camera->y);
	}

private:
	double xpos;
	double ypos;
};


// stores the rendering of sprites values
// TODO make a variable pointer for Renderer and avoid having to input it in every render function
class SpriteRenderer
{
public:
	SpriteRenderer(double x, double y, int w, int h, double scale, int xOffset = 0, int yOffset = 0) :
		texture(NULL),
		xPos(x),
		yPos(y),
		width(w),
		height(h),
		scale(scale),
		xOffset(xOffset),
		yOffset(yOffset),
		dstQuad({ (int)xPos, (int)yPos, (int)(width * scale), (int)(height * scale) })
	{}
	virtual ~SpriteRenderer() {}

	void moveSprite(double x, double y);

	// define the quads for each sprite
	void virtual defineSrcSprites() {}

	void loadmedia(SDL_Renderer* _renderer, std::string path);

	// calculates the quad renders a copy based on the gameObjects position
	void renderSprite(SDL_Renderer* renderer, SDL_Rect* srcQuad, SDL_RendererFlip flip, SDL_Rect* camera, int offsetx = 0, int offsety = 0);

	// render logic at each frame
	void virtual render(SDL_Renderer* renderer, SDL_Rect* camera) = 0;

	SDL_Texture* getTexture() {
		return texture;
	}
	double getSpritePosx() { return xPos; }
	double getSpritePosy() { return yPos; }
	void setSpritePosx(double x) { xPos = x + (xOffset * scale); }
	void setSpritePosy(double y) { yPos = y + (xOffset * scale); }

	int getSpriteWidth() { return width; }
	int getSpriteHeight() { return height; }


private:
	double xPos;
	double yPos;

	int width;
	int height;

	double scale;

	int xOffset;
	int yOffset;

	SDL_Texture* texture;

	SDL_Rect dstQuad;
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
	Collider(int centerx, int centery, int w, int h, CollisionType t, int xOffset = 0, int yOffset = 0) :
		type(t),
		centerx(centerx+xOffset),
		centery(centery+yOffset),
		halfWidth(w / 2),
		halfHeight(h / 2),
		prevCenterx(centerx),
		prevCentery(centery),
		xOffset(xOffset),
		yOffset(yOffset)
	{}
	virtual ~Collider() {}

	// returns true if the object is colliding with the specified object
	bool isColliding(Collider* c);

	// draws the hitbox using 4 lines
	void drawCollisionBox(SDL_Renderer* _renderer, SDL_Rect* camera);

	// places the center at specified position
	void setColliderCenter(int x, int y);

	// move the collider by specified difference
	void moveCollider(int dx, int dy) {
		centerx += dx;
		centery += dy;
	}

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
	int getPrevCenterx() { return prevCenterx; }
	int getPrevCentery() { return prevCentery; }
	int getColliderOffsetx() { return xOffset; }
	int getColliderOffsety() { return yOffset; }

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

	int xOffset;
	int yOffset;
};



#endif /*GAMEOBJECT_H_*/