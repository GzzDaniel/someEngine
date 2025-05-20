#ifndef SECONDARYENDITIES_H_
#define SECONDARYENDITIES_H_
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>

#include "gameObject.h"
#include "controllermanager.h"
#include "player.h"

class Obstacle : public GameObject, public ColliderManager, public SpriteRenderer
{
public:
	Obstacle(int x, int y) :
		GameObject(x, y),
		SpriteRenderer(x-50, y-50, 100, 100, 1)
	{
		Collider c(x, y, 80, 80, TYPE_PUSHOUT);
		addNewCollider(0, c);
	}
	~Obstacle() {}
	void defineSrcSprites() {
		quad = { 126, 64, 32, 32 };
	}
	void update() override {}
	void render(SDL_Renderer* _renderer, SDL_Rect* camera) override
	{ //renderSprite(_renderer, &quad, SDL_FLIP_NONE); 
		//renderSprite(_renderer, &quad, SDL_FLIP_NONE, camera);
		//drawCollisionBox(_renderer, camera);
	}

private:
	SDL_Rect quad;
};
class Table : public GameObject, public ColliderManager, public SpriteRenderer
{
public:
	Table(int x, int y) :
		GameObject(x, y),
		
		SpriteRenderer(x, y, 48, 32, 2, -24, -12)
	{
		Collider c(x, y, 48 * 2, 19, TYPE_PUSHOUT, 0, 10);
		addNewCollider(0, c);
	}
	~Table() {}
	void defineSrcSprites() {
	}
	void update() override {
		setYcamValue((int)getyPos());
	}
	void render(SDL_Renderer* _renderer, SDL_Rect* camera) override
	{ 
		renderSprite(_renderer, 0, SDL_FLIP_NONE, camera);
		drawCollisionBoxes(_renderer, camera);
		drawGOPoint(_renderer, camera);
	}

};

class Background : public SpriteRenderer
{
public:
	Background() : SpriteRenderer(0, 0, 900, 600, 1, 0, 0, -100) { }
	~Background() {}
	void render(SDL_Renderer* _renderer, SDL_Rect* camera) override {
		renderSprite(_renderer, 0, SDL_FLIP_NONE, camera);
	}
};



#endif /*SECONDARYENDITIES_H_*/

