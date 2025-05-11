#include <Windows.h>
#include <gl/GLU.h>
#include <assert.h>
#include "SOIL.h"

#include "Potion.h"
#include "defs.h"


typedef struct potion_t
{
	Object obj;

	bool isCollected;
	bool isUsed;
	uint32_t damage;
	GLuint potionTexture;
} Potion;

static void _potionUpdate(Object* obj, uint32_t milliseconds);
static void _potionDraw(Object* obj);
static ObjVtable _potionVtable = {
	_potionDraw,
	_potionUpdate
};

void potionInitTextures(Potion* potion)
{
	if (potion->potionTexture == 0)
	{
		potion->potionTexture = SOIL_load_OGL_texture(COLLECTIBLES_SPRITESHEET, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(potion->potionTexture != 0);
	}
}

Potion* potionNew(Coord2D pos, uint32_t _damage)
{
	Potion* potion = malloc(sizeof(Potion));
	if (potion != NULL)
	{
		Coord2D velocity = { 0.0f, 0.0f };
		Coord2D size = { TILE_SIZE, TILE_SIZE };

		objInit(&potion->obj, &_potionVtable, pos, size, velocity);

		potion->isCollected = false;
		potion->isUsed = false;
		potion->damage = _damage;
		potion->potionTexture = 0;
		potionInitTextures(potion);
	}

	return potion;
}

void potionDelete(Potion* potion)
{
	objDeinit(&potion->obj);

	free(potion);
}

void potionSetCollected(Potion* potion)
{
	potion->isCollected = true;
}

void potionSetUsed(Potion* potion)
{
	potion->isUsed = true;
}

Coord2D potionGetPos(Potion* potion)
{
	return potion->obj.position;
}

bool potionCheckCollected(Potion* potion)
{
	return potion->isCollected;
}

bool potionCheckUsed(Potion* potion)
{
	return potion->isUsed;
}

bool potionCheckCollision(Potion* potion, Object* player)
{
	float xBoundLeft = potion->obj.position.x - potion->obj.size.x / 2.1f;
	float xBoundRight = potion->obj.position.x + potion->obj.size.x / 2.1f;
	float yBoundTop = potion->obj.position.y - potion->obj.size.x / 2.1f;
	float yBoundBottom = potion->obj.position.y + potion->obj.size.x / 2.1f;

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

static void _potionDraw(Object* obj)
{
	Potion* potion = (Potion*)obj;

	if (!potion->isCollected)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, potion->potionTexture);
		// disable anti-aliasing
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = (obj->position.x - potion->obj.size.x / 2);
			GLfloat xPosRight = (obj->position.x + potion->obj.size.x / 2);
			GLfloat yPosTop = (obj->position.y - potion->obj.size.y / 2);
			GLfloat yPosBottom = (obj->position.y + potion->obj.size.y / 2);

			float uPerChar = 1.0f / COLLECTIBLES_COUNT;

			GLfloat xTexCoord = 3 * uPerChar;

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
}

static void _potionUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);
}