// use combat systems like health, damage, status effects, enemy-player behaviors

#ifndef COMBAT_H_
#define COMBAT_H_
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cmath>

#include "gameObject.h"
#include "controllermanager.h"
#include "player.h"


class CombatObject
{
public:
    CombatObject();
    virtual ~CombatObject() = default;

    // Combat
    virtual void takeDamage(int damage);
    virtual void attack() = 0;

    // Health
    int getHealth() const;
    int getMaxHealth() const;

    // Damage
    int getAttackDamage() const;

    // Invincibility
    bool isInvincible() const;
    void setInvincible(bool value);

    // Temporary invulnerability
    bool isInvulnerable() const;
    void setInvulnerable(bool value);

    // Invulnerability frames
    void startInvulnerability(int frames);
    void updateInvulnerability();

    // Death
    bool isDead() const;

protected:
    int health;
    int maxHealth;
    int attackDamage;

    bool invincible;
    bool invulnerable;

    int invulnerabilityTimer;

    bool dead;
};


#endif /*COMBAT_H_*/