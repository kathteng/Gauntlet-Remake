#include <Windows.h>
#include <gl/GLU.h>
#include <assert.h>
#include "SOIL.h"

#include "Food.h"
#include "defs.h"


typedef struct food_t
{
	Object obj;

	uint32_t heal;
	GLuint foodTexture;
} Food;

static void _foodUpdate(Object* obj, uint32_t milliseconds);
static void _foodDraw(Object* obj);
static ObjVtable _foodVtable = {
	_foodDraw,
	_foodUpdate
};

void foodInitTextures(Food* food)
{
	if (food->foodTexture == 0)
	{
		food->foodTexture = SOIL_load_OGL_texture(COLLECTIBLES_SPRITESHEET, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(food->foodTexture != 0);
	}
}

Food* foodNew(Coord2D pos, uint32_t health)
{
	Food* food = malloc(sizeof(Food));
	if (food != NULL)
	{
		Coord2D velocity = { 0.0f, 0.0f };
		Coord2D size = { TILE_SIZE, TILE_SIZE };

		objInit(&food->obj, &_foodVtable, pos, size, velocity);
		food->heal = health;
		food->foodTexture = 0;
		foodInitTextures(food);
	}

	return food;
}

void foodDelete(Food* food)
{
	objDeinit(&food->obj);

	free(food);
}

uint32_t foodGetHealth(Food* food)
{
	return food->heal;
}

Coord2D foodGetPos(Food* food)
{
	return food->obj.position;
}

bool foodCheckCollision(Food* food, Object* player)
{
	float xBoundLeft = food->obj.position.x - food->obj.size.x / 2.1f;
	float xBoundRight = food->obj.position.x + food->obj.size.x / 2.1f;
	float yBoundTop = food->obj.position.y - food->obj.size.x / 2.1f;
	float yBoundBottom = food->obj.position.y + food->obj.size.x / 2.1f;

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

static void _foodDraw(Object* obj)
{
	Food* food = (Food*)obj;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, food->foodTexture);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = (obj->position.x - food->obj.size.x / 2);
		GLfloat xPosRight = (obj->position.x + food->obj.size.x / 2);
		GLfloat yPosTop = (obj->position.y - food->obj.size.y / 2);
		GLfloat yPosBottom = (obj->position.y + food->obj.size.y / 2);

		float uPerChar = 1.0f / COLLECTIBLES_COUNT;

		GLfloat xTexCoord = 2 * uPerChar;

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

static void _foodUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);
}