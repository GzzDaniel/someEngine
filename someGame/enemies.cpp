#ifndef ENEMIES_H_
#define ENEMIES_H_
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>

#include "gameObject.h"
#include "controllermanager.h"
#include "player.h"


class Enemy : public GameObject, public ColliderManager
{
public:
	Enemy() {}
	~Enemy() {}
private:

};




#endif /*ENEMIES_H_*/