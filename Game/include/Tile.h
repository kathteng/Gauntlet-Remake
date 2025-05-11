#pragma once
#include <Windows.h>
#include <gl/GLU.h>
#include "baseTypes.h"
#include "Object.h"

typedef struct tile_t Tile;

typedef struct tile_t
{
	Object obj;

	char* tileSprite;
	uint16_t asciiChar;
	GLuint tileTexture;
} Tile;

void tileInitTextures(Tile* tile);
Tile* tileNew(Coord2D pos, char* fileName, uint16_t _asciiChar);
void tileDelete(Tile* tile);
uint16_t tileGetType(Tile* tile);
void tileDraw(Object* obj);