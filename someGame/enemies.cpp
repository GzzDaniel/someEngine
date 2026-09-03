// use combat systems like health, damage, status effects, enemy-player behaviors

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
#include "combat.h"

class GenericEnemy : public GameObject, public ColliderManager, public SpriteRenderer, public CombatObject
{
	GenericEnemy(int x, int y) :
		GameObject(x, y),
		SpriteRenderer(x, y, 32, 32, 2, -16, -16)
	{
		Collider c(x, y, 32 * 2, 32 * 2, TYPE_ENEMY);
		addNewCollider(0, c);
	}	


};





#endif /*ENEMIES_H_*/