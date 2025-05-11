#pragma once
#include "baseTypes.h"
#include "Object.h"

typedef struct walltile_t WallTile;

WallTile* wallTileNew(Coord2D pos, uint16_t _asciiChar, uint8_t _dir);
void wallTileDelete(WallTile* wall);
bool wallCheckCollision(Object* obj, WallTile* wall);