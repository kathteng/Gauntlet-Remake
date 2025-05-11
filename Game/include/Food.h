#pragma once
#include "baseTypes.h"
#include "Object.h"


typedef struct food_t Food;

void foodInitTextures(Food* food);
Food* foodNew(Coord2D pos, uint32_t health);
void foodDelete(Food* food);
uint32_t foodGetHealth(Food* food);
Coord2D foodGetPos(Food* food);
bool foodCheckCollision(Food* food, Object* player);