// use combat systems like health, damage, status effects, enemy-player behaviors

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

CombatObject::CombatObject()
    : health(10),
    maxHealth(10),
    attackDamage(1),
    invincible(false),
    invulnerable(false),
    invulnerabilityTimer(0),
    dead(false)
{
}

void CombatObject::takeDamage(int damage)
{
    // Cannot take damage while permanently invincible
    if (invincible)
        return;

    // Cannot take damage during temporary invulnerability
    if (invulnerable)
        return;

    // Already dead
    if (dead)
        return;

    health -= damage;

    if (health <= 0)
    {
        health = 0;
        dead = true;
    }
    else
    {
        // Start temporary invulnerability after being hit
        startInvulnerability(10);
    }
}

int CombatObject::getHealth() const
{
    return health;
}

int CombatObject::getMaxHealth() const
{
    return maxHealth;
}

int CombatObject::getAttackDamage() const
{
    return attackDamage;
}

bool CombatObject::isInvincible() const
{
    return invincible;
}

void CombatObject::setInvincible(bool value)
{
    invincible = value;
}

bool CombatObject::isInvulnerable() const
{
    return invulnerable;
}

void CombatObject::setInvulnerable(bool value)
{
    invulnerable = value;

    if (!value)
        invulnerabilityTimer = 0;
}

void CombatObject::startInvulnerability(int frames)
{
    invulnerable = true;
    invulnerabilityTimer = frames;
}

void CombatObject::updateInvulnerability()
{
    if (!invulnerable)
        return;

    if (invulnerabilityTimer > 0)
    {
        invulnerabilityTimer--;
    }

    if (invulnerabilityTimer <= 0)
    {
        invulnerabilityTimer = 0;
        invulnerable = false;
    }
}

bool CombatObject::isDead() const
{
    return dead;
}
