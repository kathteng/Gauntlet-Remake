#pragma once
#include "baseTypes.h"
#include "Object.h"


typedef struct treasure_t Treasure;

void treasureInitTextures(Treasure* treasure);
Treasure* treasureNew(Coord2D pos, uint32_t _points);
void treasureDelete(Treasure* treasure);
uint32_t treasureGetPoints(Treasure* treasure);
Coord2D treasureGetPos(Treasure* treasure);
bool treasureCheckCollision(Treasure* treasure, Object* player);