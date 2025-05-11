#include <Windows.h>
#include <gl/GLU.h>
#include <assert.h>
#include "SOIL.h"

#include "Key.h"
#include "defs.h"

typedef struct key_t
{
	Object obj;

	bool isPickedUp;
	GLuint keyTexture;
} Key;

static void _keyUpdate(Object* obj, uint32_t milliseconds);
static void _keyDraw(Object* obj);
static ObjVtable _keyVtable = {
	_keyDraw,
	_keyUpdate
};

void keyInitTextures(Key* key)
{
	if (key->keyTexture == 0)
	{
		key->keyTexture = SOIL_load_OGL_texture(COLLECTIBLES_SPRITESHEET, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(key->keyTexture != 0);
	}
}

Key* keyNew(Coord2D pos)
{
	Key* key = malloc(sizeof(Key));
	if (key != NULL)
	{
		Coord2D velocity = { 0.0f, 0.0f };
		Coord2D size = { TILE_SIZE, TILE_SIZE };

		objInit(&key->obj, &_keyVtable, pos, size, velocity);
		key->isPickedUp = false;
		key->keyTexture = 0;
		keyInitTextures(key);
	}

	return key;
}

void keyDelete(Key* key)
{
	objDeinit(&key->obj);

	free(key);
}

void keySetPickedUp(Key* key)
{
	key->isPickedUp = true;
}

Coord2D keyGetPos(Key* key)
{
	return key->obj.position;
}

bool keyCheckCollision(Key* key, Object* player)
{
	float xBoundLeft = key->obj.position.x - key->obj.size.x / 2.1f;
	float xBoundRight = key->obj.position.x + key->obj.size.x / 2.1f;
	float yBoundTop = key->obj.position.y - key->obj.size.x / 2.1f;
	float yBoundBottom = key->obj.position.y + key->obj.size.x / 2.1f;

	float objXLeft = player->position.x - player->size.x / 2.0f;
	float objXRight = player->position.x + player->size.x / 2.0f;
	float objYTop = player->position.y - player->size.y / 2.0f;
	float objYBottom = player->position.y + player->size.y / 2.0f;

	if (objXRight > xBoundLeft && objXLeft < xBoundRight && objYTop < yBoundBottom && objYBottom > yBoundTop)
	{
		key->obj.position.x = 0;
		key->obj.position.y = 0;
		return true;
	}

	return false;
}

static void _keyDraw(Object* obj)
{
	Key* key = (Key*)obj;

	if (!key->isPickedUp)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, key->keyTexture);
		// disable anti-aliasing
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = (obj->position.x - key->obj.size.x / 2);
			GLfloat xPosRight = (obj->position.x + key->obj.size.x / 2);
			GLfloat yPosTop = (obj->position.y - key->obj.size.y / 2);
			GLfloat yPosBottom = (obj->position.y + key->obj.size.y / 2);

			float uPerChar = 1.0f / COLLECTIBLES_COUNT;

			GLfloat xTexCoord = 0.0f;

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

static void _keyUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);
}