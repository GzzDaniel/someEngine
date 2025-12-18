#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>
#include <vector>
#include <map>

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

	// shows a point where the x and y position values for the gameobject object is
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
	SpriteRenderer(double x, double y, int w, int h, double scale, int xOffset = 0, int yOffset = 0, int Ycam = 10000) :
		texture(NULL),
		xPos(x),
		yPos(y),
		width(w),
		height(h),
		scale(scale),
		xOffset(xOffset),
		yOffset(yOffset),
		YcamValue(Ycam),
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

	double getYcamValue() const { return YcamValue; }
	void setYcamValue(int n) { YcamValue = n; }

private:

	double xPos;
	double yPos;

	int width;
	int height;

	double scale;

	int YcamValue;

	int xOffset;
	int yOffset;

	SDL_Texture* texture;

	SDL_Rect dstQuad;
};



enum CollisionType {
	// Entities
	TYPE_PLAYER,
	TYPE_WALL,
	TYPE_ENEMY,

	// Functionalities
	TYPE_PUSHOUT,
	TYPE_PUSHIN,

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
		prevCenterx(centerx + xOffset),
		prevCentery(centery + yOffset),
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

	// handle collision with other colliders, to be used inside child classes
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

// Manages the different collider arrays for an object, a map stores the different arrays
class ColliderManager
{
public:
	ColliderManager() : activeArray(nullptr) {}
	virtual ~ColliderManager() {}

	void addNewCollider(int id, Collider c) {
		colliders[id].push_back(c);
		if (!activeArray) {
			activeArray = &colliders[id];
		}
	}
	void changeCollider(int id) {
		activeArray = &colliders[id];
	}
	std::vector<Collider>* getActiveArray() {
		return activeArray;
	}

	// handle collision with other colliders, to be used inside child classes
	void virtual onCollision(Collider* pc1, Collider* pc2) {}

	void areColliding(ColliderManager* cm)
	{
		for (Collider &collider1 : *activeArray) {
			for (Collider &collider2 : *cm->getActiveArray()) {
				if (collider1.isColliding(&collider2)) {
					onCollision(&collider1, &collider2);
					// TODO maybe a return or break 
				}
			}
		}
	}
	// sets collider based on a point colculating the proper location based on the offset
	void setColliderArrayCenter(int x, int y, bool applyOffset = true)
	{
		// iterate through int, vector pairs
		for (auto& pair : colliders) {
			// iterate through array
			for (auto &col : pair.second) {
				if (applyOffset) {
					//col.setColliderCenter(x + col.getColliderOffsetx(), y + col.getColliderOffsety());
					col.setColliderCenter(x + col.getColliderOffsetx(), y + col.getColliderOffsety());
				}
				else {
					col.setColliderCenter(x, y);
				}
			}
		}
	}

	void drawCollisionBoxes(SDL_Renderer* renderer, SDL_Rect* camera) {
		for (auto& col : *activeArray) {
			col.drawCollisionBox(renderer, camera);
		}
	}

private:
	std::map<int, std::vector<Collider> > colliders;
	std::vector<Collider>* activeArray;

};


class Tilemap
{
public:
	Tilemap() {};
	virtual ~Tilemap() {};

private:
	std::vector<std::vector<GameObject*>> tileArray;
};






#endif /*GAMEOBJECT_H_*/