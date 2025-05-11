#pragma once
#include "baseTypes.h"
#include "Object.h"

typedef struct projectile_t Projectile;

void projectileInitTextures(Projectile* projectile);
Projectile* projectileNew(uint32_t _character, uint32_t _direction, uint32_t _damage, Coord2D _pos);
void projectileDelete(Projectile* projectile);
bool projectileCheckActive(Projectile* projectile);
void projectileDeactivate(Projectile* projectile);
void projectileSpawn(Projectile* projectile, uint8_t dir, Coord2D pos);
uint32_t projectileGetDamage(Projectile* projectile);