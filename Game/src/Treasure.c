#include <Windows.h>
#include <gl/GLU.h>
#include <assert.h>
#include "SOIL.h"

#include "Treasure.h"
#include "defs.h"


typedef struct treasure_t
{
	Object obj;

	uint32_t points;
	GLuint treasureTexture;
} Treasure;

static void _treasureUpdate(Object* obj, uint32_t milliseconds);
static void _treasureDraw(Object* obj);
static ObjVtable _treasureVtable = {
	_treasureDraw,
	_treasureUpdate
};

void treasureInitTextures(Treasure* treasure)
{
	if (treasure->treasureTexture == 0)
	{
		treasure->treasureTexture = SOIL_load_OGL_texture(COLLECTIBLES_SPRITESHEET, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(treasure->treasureTexture != 0);
	}
}

Treasure* treasureNew(Coord2D pos, uint32_t _points)
{
	Treasure* treasure = malloc(sizeof(Treasure));
	if (treasure != NULL)
	{
		Coord2D velocity = { 0.0f, 0.0f };
		Coord2D size = { TILE_SIZE, TILE_SIZE };

		objInit(&treasure->obj, &_treasureVtable, pos, size, velocity);

		treasure->points = _points;
		treasure->treasureTexture = 0;
		treasureInitTextures(treasure);
	}

	return treasure;
}

void treasureDelete(Treasure* treasure)
{
	objDeinit(&treasure->obj);

	free(treasure);
}

uint32_t treasureGetPoints(Treasure* treasure)
{
	return treasure->points;
}

Coord2D treasureGetPos(Treasure* treasure)
{
	return treasure->obj.position;
}

bool treasureCheckCollision(Treasure* treasure, Object* player)
{
	float xBoundLeft = treasure->obj.position.x - treasure->obj.size.x / 2.1f;
	float xBoundRight = treasure->obj.position.x + treasure->obj.size.x / 2.1f;
	float yBoundTop = treasure->obj.position.y - treasure->obj.size.x / 2.1f;
	float yBoundBottom = treasure->obj.position.y + treasure->obj.size.x / 2.1f;

	float objXLeft = player->position.x - player->size.x / 2.0f;
	float objXRight = player->position.x + player->size.x / 2.0f;
	float objYTop = player->position.y - player->size.y / 2.0f;
	float objYBottom = player->position.y + player->size.y / 2.0f;

	if (objXRight > xBoundLeft && objXLeft < xBoundRight && objYTop < yBoundBottom && objYBottom > yBoundTop)
	{
		return true;
	}

	return false;
}

static void _treasureDraw(Object* obj)
{
	Treasure* treasure = (Treasure*)obj;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, treasure->treasureTexture);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = (obj->position.x - treasure->obj.size.x / 2);
		GLfloat xPosRight = (obj->position.x + treasure->obj.size.x / 2);
		GLfloat yPosTop = (obj->position.y - treasure->obj.size.y / 2);
		GLfloat yPosBottom = (obj->position.y + treasure->obj.size.y / 2);

		float uPerChar = 1.0f / COLLECTIBLES_COUNT;

		GLfloat xTexCoord = uPerChar;

		const float BG_DEPTH = 0.0f;

		//tex coords are bottom left to top right
		// draw the textured quad as a tristrip
		glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

		// TL
		glTexCoord2f(xTexCoord, 1.0f);
		glVertex3f(xPosLeft, yPosTop, BG_DEPTH);

		// BL
		glTexCoord2f(xTexCoord, 0.0f);
		glVertex3f(xPosLeft, yPosBottom, BG_DEPTH);

		// TR
		glTexCoord2f(xTexCoord + uPerChar, 1.0f);
		glVertex3f(xPosRight, yPosTop, BG_DEPTH);

		// BR
		glTexCoord2f(xTexCoord + uPerChar, 0.0f);
		glVertex3f(xPosRight, yPosBottom, BG_DEPTH);
	}
	glEnd();
}

static void _treasureUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);
}