#pragma once
#include "baseTypes.h"
#include "Object.h"

typedef struct groundtile_t GroundTile;

GroundTile* groundTileNew(Coord2D pos, uint16_t _asciiChar);
void groundTileDelete(GroundTile* ground);