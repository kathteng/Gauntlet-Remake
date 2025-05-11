#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <gl/GLU.h>
#include "SOIL.h"

#include "baseTypes.h"
#include "defs.h"
#include "Tile.h"



void tileInitTextures(Tile* tile)
{
	if (tile->tileTexture == 0)
	{
		tile->tileTexture = SOIL_load_OGL_texture(tile->tileSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(tile->tileTexture != 0);
	}
}

Tile* tileNew(Coord2D pos, char* fileName, uint16_t _asciiChar)
{
	Tile* tile = malloc(sizeof(Tile));
	if (tile != NULL)
	{
		Coord2D _size = { TILE_SIZE, TILE_SIZE };
		Coord2D vel = { 0.0f, 0.0f };

		tile->obj.position = pos;
		tile->obj.size = _size;
		tile->obj.velocity = vel;

		tile->tileTexture = 0;
		tile->asciiChar = _asciiChar;
		tile->tileSprite = fileName;

		tileInitTextures(tile);
	}

	return tile;
}

void tileDelete(Tile* tile)
{
	free(tile);
}

uint16_t tileGetType(Tile* tile)
{
	return tile->asciiChar;
}

void tileDraw(Object* obj)
{
	Tile* tile = (Tile*)obj;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tile->tileTexture);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = (obj->position.x - tile->obj.size.x / 2);
		GLfloat xPosRight = (obj->position.x + tile->obj.size.x / 2);
		GLfloat yPosTop = (obj->position.y - tile->obj.size.y / 2);
		GLfloat yPosBottom = (obj->position.y + tile->obj.size.y / 2);

		const float BG_DEPTH = -0.99f;

		//tex coords are bottom left to top right
		// draw the textured quad as a tristrip
		glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

		// TL
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(xPosLeft, yPosTop, BG_DEPTH);

		// BL
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(xPosLeft, yPosBottom, BG_DEPTH);

		// TR
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(xPosRight, yPosTop, BG_DEPTH);

		// BR
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(xPosRight, yPosBottom, BG_DEPTH);
	}
	glEnd();
}