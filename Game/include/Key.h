#pragma once
#include "baseTypes.h"
#include "Object.h"


typedef struct key_t Key;

void keyInitTextures(Key* key);
Key* keyNew(Coord2D pos);
void keyDelete(Key* key);
void keySetPickedUp(Key* key);
Coord2D keyGetPos(Key* key);
bool keyCheckCollision(Key* key, Object* player);