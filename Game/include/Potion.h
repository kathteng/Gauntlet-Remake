#pragma once
#include "baseTypes.h"
#include "Object.h"


typedef struct potion_t Potion;

void potionInitTextures(Potion* potion);
Potion* potionNew(Coord2D pos, uint32_t _damage);
void potionDelete(Potion* potion);
void potionSetCollected(Potion* potion);
void potionSetUsed(Potion* potion);
Coord2D potionGetPos(Potion* potion);

bool potionCheckCollected(Potion* potion);
bool potionCheckUsed(Potion* potion);
bool potionCheckCollision(Potion* potion, Object* player);