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

class Obstacle : public GameObject, public Collider, public SpriteRenderer
{
public:
	Obstacle(int x, int y) :
		GameObject(x, y),
		Collider(x, y, 80, 80, TYPE_PUSHOUT),
		SpriteRenderer(x-50, y-50, 100, 100, 1)
	{}
	~Obstacle() {}
	void defineSrcSprites() {
		quad = { 126, 64, 32, 32 };
	}
	void update() override {}
	void render(SDL_Renderer* _renderer, SDL_Rect* camera) override
	{ //renderSprite(_renderer, &quad, SDL_FLIP_NONE); 
		renderSprite(_renderer, &quad, SDL_FLIP_NONE, camera);
		//drawCollisionBox(_renderer, camera);
	}

private:
	SDL_Rect quad;
};
class Background : public SpriteRenderer
{
public:
	Background() : SpriteRenderer(0, 0, 900, 600, 1) { }
	~Background() {}
	void render(SDL_Renderer* _renderer, SDL_Rect* camera) override {
		renderSprite(_renderer, 0, SDL_FLIP_NONE, camera);
	}
};



#endif /*SECONDARYENDITIES_H_*/

