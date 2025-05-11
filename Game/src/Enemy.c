#include <Windows.h>
#include <gl/GLU.h>
#include <assert.h>
#include "SOIL.h"

#include "Enemy.h"
#include "Level.h"
#include "defs.h"



typedef struct enemy_t
{
	Entity entity;
	Node* node;

	EnemyType enemyType;
	Coord2D prevPos;
	uint32_t points;
	uint32_t damage;
	float speed;
	bool isDead;
	bool isMoving;
	float frameTimer;
	uint8_t currFrame;
	GLuint enemyTexture;
	char* enemySprite;
} Enemy;

static char GHOST_SPRITE[] = "asset/enemies/ghost_spritesheet.png";
static char GRUNT_SPRITE[] = "asset/enemies/grunt_spritesheet.png";
static char DEATH_SPRITE[] = "asset/enemies/death_spritesheet.png";

static float pathRecalc = 0.0f;

static void _enemyUpdate(Object* obj, uint32_t milliseconds);
static void _enemyDraw(Object* obj);
static ObjVtable _enemyVtable = {
	_enemyDraw,
	_enemyUpdate
};

void enemyInitTextures(Enemy* enemy)
{
	if (enemy->enemyTexture == 0)
	{
		enemy->enemyTexture = SOIL_load_OGL_texture(enemy->enemySprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(enemy->enemyTexture != 0);
	}
}

Enemy* enemyNew(Coord2D pos, uint8_t type)
{
	Enemy* enemy = malloc(sizeof(Enemy));
	if (enemy != NULL)
	{
		Coord2D velocity = { 0.0f, 0.0f };
		Coord2D size = { TILE_SIZE, TILE_SIZE };
		uint32_t health = 0;

		if (type == 0) // ghost
		{
			enemy->enemySprite = GHOST_SPRITE;
			enemy->enemyType = GHOST;
			enemy->points = 5;
			enemy->damage = 1;
			enemy->speed = 0.1f;
			health = 10;
		}
		else if (type == 1) // grunt
		{
			enemy->enemySprite = GRUNT_SPRITE;
			enemy->enemyType = GRUNT;
			enemy->points = 20;
			enemy->damage = 5;
			enemy->speed = 0.1f;
			health = 20;
		}
		else if (type == 2) // death
		{
			enemy->enemySprite = DEATH_SPRITE;
			enemy->enemyType = DEATH;
			enemy->points = 200;
			enemy->damage = 200;
			enemy->speed = 0.1f;
			health = 200;
		}

		enemy->entity = *entityNew(health);
		objInit(&enemy->entity.obj, &_enemyVtable, pos, size, velocity);

		// grid pos
		uint32_t x = (uint32_t)(enemy->entity.obj.position.x / TILE_SIZE);
		uint32_t y = (uint32_t)(enemy->entity.obj.position.y / TILE_SIZE);
		enemy->node = nodeNew(NULL, x, y, 0.0f, 0.0f, 0.0f, enemy->enemyType);
		
		enemy->prevPos = pos;
		enemy->isDead = false;
		enemy->isMoving = true;
		enemy->frameTimer = 0.0f;
		enemy->currFrame = 0;
		enemy->enemyTexture = 0;
		enemyInitTextures(enemy);
	}

	return enemy;
}

void enemyDelete(Enemy* enemy)
{
	entityDelete(&enemy->entity);
	objDeinit(&enemy->entity.obj);

	//free(enemy);
}

void enemySetDirection(Enemy* enemy, uint8_t dir)
{
	enemy->entity.currDir = dir;
}

void enemyResetPos(Enemy* enemy)
{
	enemy->entity.obj.position = enemy->prevPos;
}

bool enemyCheckDead(Enemy* enemy)
{
	return enemy->isDead;
}

Coord2D enemyGetPos(Enemy* enemy)
{
	return enemy->entity.obj.position;
}

uint32_t enemyGetDamage(Enemy* enemy)
{
	return enemy->damage;
}

uint32_t enemyTakeDamage(Enemy* enemy, uint32_t damage)
{
	uint32_t points = 0;
	enemy->entity.currHealth -= damage;
	if (enemy->entity.currHealth <= 0)
	{
		points = enemy->points;
		enemy->isDead = true;
	}

	return points;
}

bool enemyCheckCollision(Enemy* enemy, Object* obj)
{
	float xBoundLeft = enemy->entity.obj.position.x - enemy->entity.obj.size.x / 2.1f;
	float xBoundRight = enemy->entity.obj.position.x + enemy->entity.obj.size.x / 2.1f;
	float yBoundTop = enemy->entity.obj.position.y - enemy->entity.obj.size.x / 2.1f;
	float yBoundBottom = enemy->entity.obj.position.y + enemy->entity.obj.size.x / 2.1f;

	float objXLeft = obj->position.x - obj->size.x / 2.0f;
	float objXRight = obj->position.x + obj->size.x / 2.0f;
	float objYTop = obj->position.y - obj->size.y / 2.0f;
	float objYBottom = obj->position.y + obj->size.y / 2.0f;

	if (objXRight > xBoundLeft && objXLeft < xBoundRight && objYTop < yBoundBottom && objYBottom > yBoundTop)
	{
		return true;
	}

	return false;
}

void enemyUpdatePath(Enemy* enemy)
{
	uint32_t x = (uint32_t)(enemy->entity.obj.position.x / TILE_SIZE);
	uint32_t y = (uint32_t)(enemy->entity.obj.position.y / TILE_SIZE);
	
	uint32_t nextX = 0;
	uint32_t nextY = 0;

	Node* goal = selectTarget(enemy->node);
	
	if (aStar(enemy->node, goal, &nextX, &nextY))
	{
		enemy->isMoving = true;
		if (nextX > x && nextY > y)
		{
			enemySetDirection(enemy, SOUTHEAST);
		}
		else if (nextX < x && nextY > y)
		{
			enemySetDirection(enemy, SOUTHWEST);
		}
		else if (nextX < x && nextY < y)
		{
			enemySetDirection(enemy, NORTHWEST);
		}
		else if (nextX > x && nextY < y)
		{
			enemySetDirection(enemy, NORTHEAST);
		}
		else if (nextX < x)
		{
			enemySetDirection(enemy, WEST);
		}
		else if (nextX > x)
		{
			enemySetDirection(enemy, EAST);
		}
		else if (nextY > y)
		{
			enemySetDirection(enemy, SOUTH);
		}
		else if (nextY < y)
		{
			enemySetDirection(enemy, NORTH);
		}
	}
	else
		enemy->isMoving = false;
}

static void _enemyDraw(Object* obj)
{
	Enemy* enemy = (Enemy*)obj;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, enemy->enemyTexture);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = (obj->position.x - enemy->entity.obj.size.x / 2);
		GLfloat xPosRight = (obj->position.x + enemy->entity.obj.size.x / 2);
		GLfloat yPosTop = (obj->position.y - enemy->entity.obj.size.y / 2);
		GLfloat yPosBottom = (obj->position.y + enemy->entity.obj.size.y / 2);

		float uPerChar = 1.0f / DIR_COUNT;
		float vPerChar = 1.0f / 3.0f;

		GLfloat xTexCoord = enemy->entity.currDir * uPerChar;
		GLfloat yTexCoord = enemy->currFrame * vPerChar;

		const float BG_DEPTH = 0.90f;

		//tex coords are bottom left to top right
		// draw the textured quad as a tristrip
		glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

		// TL
		glTexCoord2f(xTexCoord, yTexCoord + vPerChar);
		glVertex3f(xPosLeft, yPosTop, BG_DEPTH);

		// BL
		glTexCoord2f(xTexCoord, yTexCoord);
		glVertex3f(xPosLeft, yPosBottom, BG_DEPTH);

		// TR
		glTexCoord2f(xTexCoord + uPerChar, yTexCoord + vPerChar);
		glVertex3f(xPosRight, yPosTop, BG_DEPTH);

		// BR
		glTexCoord2f(xTexCoord + uPerChar, yTexCoord);
		glVertex3f(xPosRight, yPosBottom, BG_DEPTH);
	}
	glEnd();
}

static void _enemyUpdate(Object* obj, uint32_t milliseconds)
{
	Enemy* enemy = (Enemy*)obj;

	enemy->frameTimer += milliseconds;
	if (enemy->frameTimer >= ANIM_FRAME_RATE)
	{
		++enemy->currFrame;
		if (enemy->currFrame == 3)
			enemy->currFrame = 0;
		enemy->frameTimer = 0.0f;
	}
	
	if (!enemy->isMoving)
	{
		enemy->entity.obj.velocity.x = 0;
		enemy->entity.obj.velocity.y = 0;

		enemyResetPos(enemy);
	}
	else
	{
		enemy->prevPos = enemy->entity.obj.position;

		if (enemy->entity.currDir == SOUTH)
		{
			enemy->entity.obj.velocity.x = enemy->speed * 0;
			enemy->entity.obj.velocity.y = enemy->speed * 1;
		}
		else if (enemy->entity.currDir == WEST)
		{
			enemy->entity.obj.velocity.x = enemy->speed * -1;
			enemy->entity.obj.velocity.y = enemy->speed * 0;
		}
		else if (enemy->entity.currDir == NORTH)
		{
			enemy->entity.obj.velocity.x = enemy->speed * 0;
			enemy->entity.obj.velocity.y = enemy->speed * -1;
		}
		else if (enemy->entity.currDir == EAST)
		{
			enemy->entity.obj.velocity.x = enemy->speed * 1;
			enemy->entity.obj.velocity.y = enemy->speed * 0;
		}
		else if (enemy->entity.currDir == SOUTHWEST)
		{
			enemy->entity.obj.velocity.x = enemy->speed * -1 / SQRT2;
			enemy->entity.obj.velocity.y = enemy->speed * 1 / SQRT2;
		}
		else if (enemy->entity.currDir == SOUTHEAST)
		{
			enemy->entity.obj.velocity.x = enemy->speed * 1 / SQRT2;
			enemy->entity.obj.velocity.y = enemy->speed * 1 / SQRT2;
		}
		else if (enemy->entity.currDir == NORTHWEST)
		{
			enemy->entity.obj.velocity.x = enemy->speed * -1 / SQRT2;
			enemy->entity.obj.velocity.y = enemy->speed * -1 / SQRT2;
		}
		else if (enemy->entity.currDir == NORTHEAST)
		{
			enemy->entity.obj.velocity.x = enemy->speed * 1 / SQRT2;
			enemy->entity.obj.velocity.y = enemy->speed * -1 / SQRT2;
		}
	}

	objDefaultUpdate(obj, milliseconds);
	updateEnemyNodePos(enemy->node, (uint32_t)(enemy->entity.obj.position.x / TILE_SIZE), (uint32_t)(enemy->entity.obj.position.y / TILE_SIZE));

	// calculate astar path
	pathRecalc += milliseconds;
	if (pathRecalc >= 240.0f)
	{
		enemyUpdatePath(enemy);
		pathRecalc = 0.0f;
	}
}