#pragma once
#include "baseTypes.h"
#include "Object.h"


typedef struct exittile_t ExitTile;

ExitTile* exitTileNew(Coord2D pos, uint16_t _asciiChar, uint8_t level);
void exitTileDelete(ExitTile* exit);
bool exitCheckCollision(ExitTile* exit, Object* player);
uint8_t exitGetNextLevel(ExitTile* exit);