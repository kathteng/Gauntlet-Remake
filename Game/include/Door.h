#pragma once
#include "baseTypes.h"
#include "Object.h"


typedef struct door_t Door;

void doorInitTextures(Door* door);
Door* doorNew(Coord2D pos, uint8_t dir);
void doorDelete(Door* door);
void doorSetOpened(Door* door);
bool doorCheckOpened(Door* door);
Coord2D doorGetPos(Door* door);
bool doorCheckCollision(Object* obj, Door* door);