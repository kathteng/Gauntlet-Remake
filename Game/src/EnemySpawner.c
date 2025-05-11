#include <Windows.h>
#include <gl/GLU.h>
#include <assert.h>
#include "SOIL.h"

#include "EnemySpawner.h"
#include "Enemy.h"
#include "Entity.h"
#include "defs.h"
#include "random.h"
#include "Astar.h"
#include "Projectile.h"


typedef struct enemyspawner_t
{
	Entity entity;

	EnemyType enemyType;
	Enemy* enemies[MAX_ENEMIES];
	bool isDestroyed;
	float timer;
	uint32_t points;
	uint32_t numEnemies;
	uint32_t spawnInterval; // in seconds
	GLuint spawnerTexture;
} EnemySpawner;

static char SPAWNER_SPRITE[] = "asset/enemies/enemy_spawner_spritesheet.png";

static void _enemySpawnerUpdate(Object* obj, uint32_t milliseconds);
static void _enemySpawnerDraw(Object* obj);
static ObjVtable _enemySpawnerVtable = {
	_enemySpawnerDraw,
	_enemySpawnerUpdate
};

void enemySpawnerInitTextures(EnemySpawner* enemySpawn)
{
	if (enemySpawn->spawnerTexture == 0)
	{
		enemySpawn->spawnerTexture = SOIL_load_OGL_texture(SPAWNER_SPRITE, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(enemySpawn->spawnerTexture != 0);
	}
}

EnemySpawner* enemySpawnerNew(Coord2D pos, uint8_t type)
{
	EnemySpawner* enemySpawn = malloc(sizeof(EnemySpawner));
	if (enemySpawn != NULL)
	{
		Coord2D vel = { 0.0f, 0.0f };
		Coord2D size = { TILE_SIZE, TILE_SIZE };
		
		enemySpawn->entity = *entityNew(5);
		objInit(&enemySpawn->entity.obj, &_enemySpawnerVtable, pos, size, vel);

		enemySpawn->enemyType = type;
		enemySpawn->isDestroyed = false;
		enemySpawn->timer = 0.0f;
		enemySpawn->points = 100;
		enemySpawn->numEnemies = 0;
		enemySpawn->spawnerTexture = 0;

		randSetSeed();
		if (type == 0) // ghost
		{
			enemySpawn->spawnInterval = randGetInt(5, 10);
		}
		else if (type == 1) // grunt
		{
			enemySpawn->spawnInterval = randGetInt(8, 15);
		}
		// no spawner for Death

		for (uint32_t i = 0; i < MAX_ENEMIES; ++i)
		{
			enemySpawn->enemies[i] = NULL;
		}
	}

	return enemySpawn;
}

void enemySpawnerDelete(EnemySpawner* enemySpawn)
{
	for (uint32_t i = 0; i < enemySpawn->numEnemies; ++i)
	{
		if (enemySpawn->enemies[i] != NULL)
		{
			enemyDelete(enemySpawn->enemies[i]);
			enemySpawn->enemies[i] = NULL;
		}
	}

	entityDelete(&enemySpawn->entity);
	objDeinit(&enemySpawn->entity.obj);

	//free(enemySpawn);
}

void spawnEnemies(EnemySpawner* enemySpawn)
{
	uint32_t count = randGetInt(1, ENEMY_SPAWN_MAX);
	if (enemySpawn->numEnemies + count > MAX_ENEMIES)
		return;

	for (uint32_t i = 0; i < count; i++)
	{
		Coord2D pos;
		uint8_t dir = 0;
		while (true)
		{
			pos = enemySpawn->entity.obj.position;
			dir = randGetInt(0, DIR_COUNT);
			if (dir == SOUTH)
			{
				pos.y += TILE_SIZE;
			}
			else if (dir == WEST)
			{
				pos.x -= TILE_SIZE;
			}
			else if (dir == NORTH)
			{
				pos.y -= TILE_SIZE;
			}
			else if (dir == EAST)
			{
				pos.x += TILE_SIZE;
			}
			else if (dir == SOUTHWEST)
			{
				pos.x -= TILE_SIZE;
				pos.y += TILE_SIZE;
			}
			else if (dir == NORTHWEST)
			{
				pos.x -= TILE_SIZE;
				pos.y -= TILE_SIZE;
			}
			else if (dir == NORTHEAST)
			{
				pos.x += TILE_SIZE;
				pos.y -= TILE_SIZE;
			}
			else if (dir == SOUTHEAST)
			{
				pos.x += TILE_SIZE;
				pos.y += TILE_SIZE;
			}

			if (checkTileWalkable((uint32_t)(pos.x / TILE_SIZE), (uint32_t)(pos.y / TILE_SIZE)))
				break;
		}
		
		enemySpawn->enemies[i + enemySpawn->numEnemies] = enemyNew(pos, enemySpawn->enemyType);
		enemySetDirection(enemySpawn->enemies[i + enemySpawn->numEnemies], dir);
		enemyUpdatePath(enemySpawn->enemies[i + enemySpawn->numEnemies]);
	}
	enemySpawn->numEnemies += count;

	if (enemySpawn->enemyType == GHOST)
		enemySpawn->spawnInterval = randGetInt(5, 9);
	else if (enemySpawn->enemyType == GRUNT)
		enemySpawn->spawnInterval = randGetInt(6, 10);
}

void enemiesCheckCollisionPlayer(EnemySpawner* enemySpawn, Object* player)
{
	if (enemySpawn != NULL)
	{
		for (uint32_t i = 0; i < enemySpawn->numEnemies; ++i)
		{
			if (enemySpawn->enemies[i] != NULL)
			{
				if (enemyCheckCollision(enemySpawn->enemies[i], player))
				{
					Player* p = (Player*)player;
					if (p != NULL)
					{
						playerTakeDamage(p, enemyGetDamage(enemySpawn->enemies[i]));
						uint32_t points = enemyTakeDamage(enemySpawn->enemies[i], playerGetMeleeDamage(p));
						playerAddScore(p, points);
					}
				}
			}
		}
	}
}

void enemiesCheckCollisionProjectile(EnemySpawner* enemySpawn, Object* projectile, Object* player)
{
	if (enemySpawn != NULL)
	{
		for (uint32_t i = 0; i < enemySpawn->numEnemies; ++i)
		{
			if (enemySpawn->enemies[i] != NULL)
			{
				if (enemyCheckCollision(enemySpawn->enemies[i], projectile))
				{
					Projectile* p = (Projectile*)projectile;
					if (p != NULL && projectileCheckActive(p))
					{
						assert(player != NULL);

						projectileDeactivate(p);
						uint32_t points = enemyTakeDamage(enemySpawn->enemies[i], projectileGetDamage(p));
						playerAddScore((Player*)player, points);
					}
				}
			}
		}
	}
}

void enemiesCheckPotionRadius(EnemySpawner* enemySpawn, Coord2D center, Player* player)
{
	for (uint32_t i = 0; i < enemySpawn->numEnemies; ++i)
	{
		if (enemySpawn->enemies[i] != NULL)
		{
			Coord2D enemyPos = enemyGetPos(enemySpawn->enemies[i]);
			float dist2 = (enemyPos.x - center.x) * (enemyPos.x - center.x) + (enemyPos.y - center.y) * (enemyPos.y - center.y);
			if (dist2 <= POTION_RADIUS * POTION_RADIUS)
			{
				playerAddScore(player, enemyTakeDamage(enemySpawn->enemies[i], POTION_DAMAGE * playerGetMagicDamage(player)));	
			}
		}
	}
}

bool enemySpawnerCheckCollision(EnemySpawner* enemySpawn, Object* obj)
{
	float xBoundLeft = enemySpawn->entity.obj.position.x - enemySpawn->entity.obj.size.x / 2.1f;
	float xBoundRight = enemySpawn->entity.obj.position.x + enemySpawn->entity.obj.size.x / 2.1f;
	float yBoundTop = enemySpawn->entity.obj.position.y - enemySpawn->entity.obj.size.x / 2.1f;
	float yBoundBottom = enemySpawn->entity.obj.position.y + enemySpawn->entity.obj.size.x / 2.1f;

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

bool enemySpawnerCheckDestroyed(EnemySpawner* enemySpawn)
{
	return enemySpawn->isDestroyed;
}

uint32_t enemySpawnerTakeDamage(EnemySpawner* enemySpawn)
{
	uint32_t points = 0;
	--enemySpawn->entity.currHealth;
	if (enemySpawn->entity.currHealth <= 0)
	{
		points = enemySpawn->points;
		enemySpawn->isDestroyed = true;
	}

	return points;
}

static void _enemySpawnerDraw(Object* obj)
{
	EnemySpawner* enemySpawn = (EnemySpawner*)obj;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, enemySpawn->spawnerTexture);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = (obj->position.x - enemySpawn->entity.obj.size.x / 2);
		GLfloat xPosRight = (obj->position.x + enemySpawn->entity.obj.size.x / 2);
		GLfloat yPosTop = (obj->position.y - enemySpawn->entity.obj.size.y / 2);
		GLfloat yPosBottom = (obj->position.y + enemySpawn->entity.obj.size.y / 2);

		float uPerChar = 1.0f / 2.0f;

		GLfloat xTexCoord = enemySpawn->enemyType * uPerChar;

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

static void _enemySpawnerUpdate(Object* obj, uint32_t milliseconds)
{
	EnemySpawner* enemySpawn = (EnemySpawner*)obj;

	// delete dead enemies
	for (uint32_t i = 0; i < enemySpawn->numEnemies; ++i)
	{
		if (enemySpawn->enemies[i] != NULL)
		{
			if (enemyCheckDead(enemySpawn->enemies[i]))
			{
				enemyDelete(enemySpawn->enemies[i]);
				enemySpawn->enemies[i] = NULL;
			}
		}
	}

	// spawn enemies
	enemySpawn->timer += (milliseconds / 1000.0f);
	if (enemySpawn->timer >= enemySpawn->spawnInterval)
	{
		spawnEnemies(enemySpawn);
		enemySpawn->timer = 0.0f;
	}

	objDefaultUpdate(obj, milliseconds);
}