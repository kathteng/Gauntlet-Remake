#pragma once
#include "baseTypes.h"
#include "Object.h"
#include "Player.h"


typedef struct deathspawner_t DeathSpawner;

DeathSpawner* deathSpawnerNew(uint32_t _spawnChance);
void deathSpawnerDelete(DeathSpawner* deathSpawn);

void loadDeathSound();
void unloadDeathSound();

void deathSpawnerAddPos(DeathSpawner* deathSpawn, Coord2D pos);
void deathCheckCollisionPlayer(DeathSpawner* deathSpawn, Object* obj);
void deathCheckCollisionProjectile(DeathSpawner* deathSpawn, Object* obj);
void deathCheckPotionRadius(DeathSpawner* deathSpawn, Coord2D center, Player* player);
void deathSpawnerUpdate(DeathSpawner* deathSpawn, uint32_t milliseconds);