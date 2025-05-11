#pragma once
#include "baseTypes.h"
#include "defs.h"
#include "Object.h"
#include "WallTile.h"
#include "Door.h"
#include "Player.h"


typedef struct level_t Level;

WallTile* walls[LEVEL_TILES];
Door* doors[LEVEL_TILES];

Level* levelNew(uint8_t num);
void levelDelete(Level* level);

void levelSetPlayerSpawns(Level* level, Object* p1, Object* p2);

bool checkWallsCollision(Object* obj);
bool checkDoorsCollision(Object* obj);
uint8_t checkExitsCollision(Level* level, Object* player);
bool checkSpawnersCollisionPlayer(Level* level, Object* obj);
uint32_t checkSpawnersCollisionProjectile(Level* level, Projectile* projectile);
void checkEnemiesCollision(Level* level, Object* obj, Object* projectilePlayer);

bool checkKeyCollect(Level* level, Player* player1, Player* player2);
bool checkTreasureCollect(Level* level, Player* player);
bool checkFoodCollect(Level* level, Player* player);
bool checkPotionCollect(Level* level, Player* player);

void levelOpenDoors(Coord2D playerPos);
void levelUsePotion(Level* level, Player* player);

void levelDraw(Level* level);
void levelUpdate(Level* level, uint32_t milliseconds);