#pragma once
#include "baseTypes.h"
#include "Entity.h"
#include "Astar.h"


typedef enum enemytype_t EnemyType;
typedef struct enemy_t Enemy;

typedef enum enemytype_t
{
	GHOST,
	GRUNT,
	DEATH
} EnemyType;

void enemyInitTextures(Enemy* enemy);
Enemy* enemyNew(Coord2D pos, uint8_t type);
void enemyDelete(Enemy* enemy);
void enemySetDirection(Enemy* enemy, uint8_t dir);
void enemyResetPos(Enemy* enemy);
bool enemyCheckDead(Enemy* enemy);

Coord2D enemyGetPos(Enemy* enemy);
uint32_t enemyGetDamage(Enemy* enemy);
uint32_t enemyTakeDamage(Enemy* enemy, uint32_t damage);
bool enemyCheckCollision(Enemy* enemy, Object* obj);
void enemyUpdatePath(Enemy* enemy);