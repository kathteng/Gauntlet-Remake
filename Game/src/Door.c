#include <Windows.h>
#include <gl/GLU.h>
#include <assert.h>
#include "SOIL.h"

#include "Door.h"
#include "defs.h"


typedef enum doordir_t
{
	HORIZONTAL,
	VERTICAL,
	DIRCOUNT
} DoorDir;

typedef struct door_t
{
	Object obj;

	bool isOpened;
	DoorDir dir;
	GLuint doorTexture;
} Door;

static char DOOR_SPRITE[] = "asset/levels/door_spritesheet.png";

static void _doorUpdate(Object* obj, uint32_t milliseconds);
static void _doorDraw(Object* obj);
static ObjVtable _doorVtable = {
	_doorDraw,
	_doorUpdate
};

void doorInitTextures(Door* door)
{
	if (door->doorTexture == 0)
	{
		door->doorTexture = SOIL_load_OGL_texture(DOOR_SPRITE, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(door->doorTexture != 0);
	}
}

Door* doorNew(Coord2D pos, uint8_t dir)
{
	Door* door = malloc(sizeof(Door));
	if (door != NULL)
	{
		Coord2D velocity = { 0.0f, 0.0f };
		Coord2D size = { TILE_SIZE, TILE_SIZE };

		objInit(&door->obj, &_doorVtable, pos, size, velocity);
		door->dir = dir;
		door->isOpened = false;
		door->doorTexture = 0;
		doorInitTextures(door);
	}

	return door;
}

void doorDelete(Door* door)
{
	objDeinit(&door->obj);

	free(door);
}

void doorSetOpened(Door* door)
{
	door->isOpened = true;
}

bool doorCheckOpened(Door* door)
{
	return door->isOpened;
}

Coord2D doorGetPos(Door* door)
{
	return door->obj.position;
}

bool doorCheckCollision(Object* obj, Door* door)
{
	float xBoundLeft = door->obj.position.x - door->obj.size.x / 3.0f;
	float xBoundRight = door->obj.position.x + door->obj.size.x / 3.0f;
	float yBoundTop = door->obj.position.y - door->obj.size.x / 3.0f;
	float yBoundBottom = door->obj.position.y + door->obj.size.x / 3.0f;

	float objXLeft = obj->position.x - obj->size.x / 3.0f;
	float objXRight = obj->position.x + obj->size.x / 3.0f;
	float objYTop = obj->position.y - obj->size.y / 3.0f;
	float objYBottom = obj->position.y + obj->size.y / 3.0f;

	if (objXRight > xBoundLeft && objXLeft < xBoundRight && objYTop < yBoundBottom && objYBottom > yBoundTop)
	{
		return true;
	}

	return false;
}

static void _doorDraw(Object* obj)
{
	Door* door = (Door*)obj;

	if (!door->isOpened)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, door->doorTexture);
		// disable anti-aliasing
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = (obj->position.x - door->obj.size.x / 2);
			GLfloat xPosRight = (obj->position.x + door->obj.size.x / 2);
			GLfloat yPosTop = (obj->position.y - door->obj.size.y / 2);
			GLfloat yPosBottom = (obj->position.y + door->obj.size.y / 2);

			float uPerChar = 1.0f / DIRCOUNT;

			GLfloat xTexCoord = door->dir * uPerChar;

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

static void _doorUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);
}