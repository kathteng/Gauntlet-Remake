#include <stdlib.h>
#include <assert.h>

#include "DeathSpawner.h"
#include "defs.h"
#include "random.h"
#include "Enemy.h"
#include "Projectile.h"
#include "Astar.h"
#include "sound.h"


typedef struct deathspawner_t
{
	Enemy* deaths[MAX_DEATHS];
	Coord2D spawnPos[MAX_DEATHS];
	uint32_t posCount;
	uint32_t spawnChance; // between 1 and 100
} DeathSpawner;

static float timer = 0.0f;
static float damageTick = 0.0f;
static float pathRecalc = 0.0f;
static bool isHitting = true;

static int32_t deathSound = SOUND_NOSOUND;

DeathSpawner* deathSpawnerNew(uint32_t _spawnChance)
{
	DeathSpawner* deathSpawn = malloc(sizeof(DeathSpawner));
	if (deathSpawn != NULL)
	{
		deathSpawn->spawnChance = _spawnChance / 2;
		deathSpawn->posCount = 0;

		Coord2D pos = { 0.0f, 0.0f };
		for (uint32_t i = 0; i < MAX_DEATHS; ++i)
		{
			deathSpawn->deaths[i] = NULL;
			deathSpawn->spawnPos[i] = pos;
		}
	}

	return deathSpawn;
}

void deathSpawnerDelete(DeathSpawner* deathSpawn)
{
	for (uint32_t i = 0; i < MAX_DEATHS; ++i)
	{
		if (deathSpawn->deaths[i] != NULL)
		{
			enemyDelete(deathSpawn->deaths[i]);
			deathSpawn->deaths[i] = NULL;
		}
	}
	free(deathSpawn);
}

void loadDeathSound()
{
	deathSound = soundLoad("asset/sounds/death.wav");
}

void unloadDeathSound()
{
	soundUnload(deathSound);
}

void deathSpawnerAddPos(DeathSpawner* deathSpawn, Coord2D pos)
{
	for (uint32_t i = 0; i < MAX_DEATHS; ++i)
	{
		if ((uint32_t)deathSpawn->spawnPos[i].x == 0 && (uint32_t)deathSpawn->spawnPos[i].y == 0.0f)
		{
			deathSpawn->spawnPos[i] = pos;
			++deathSpawn->posCount;
			return;
		}
	}

	// out of space to add spawn pos
	assert(false);
}

void deathCheckCollisionPlayer(DeathSpawner* deathSpawn, Object* obj)
{
	for (uint32_t i = 0; i < MAX_DEATHS; ++i)
	{
		if (deathSpawn->deaths[i] != NULL)
		{
			if (enemyCheckCollision(deathSpawn->deaths[i], obj))
			{
				Player* player = (Player*)obj;
				if (player != NULL)
				{
					if (isHitting)
					{
						playerTakeDamage(player, enemyGetDamage(deathSpawn->deaths[i]));
						isHitting = false;
					}
				}
			}
		}
	}
}

void deathCheckCollisionProjectile(DeathSpawner* deathSpawn, Object* obj)
{
	for (uint32_t i = 0; i < MAX_DEATHS; ++i)
	{
		if (deathSpawn->deaths[i] != NULL)
		{
			if (enemyCheckCollision(deathSpawn->deaths[i], obj))
			{
				Projectile* projectile = (Projectile*)obj;
				if (projectile != NULL && projectileCheckActive(projectile))
				{
					projectileDeactivate(projectile);
				}
			}
		}
	}
}

void deathCheckPotionRadius(DeathSpawner* deathSpawn, Coord2D center, Player* player)
{
	for (uint32_t i = 0; i < MAX_DEATHS; ++i)
	{
		if (deathSpawn->deaths[i] != NULL)
		{
			Coord2D enemyPos = enemyGetPos(deathSpawn->deaths[i]);
			float dist2 = (enemyPos.x - center.x) * (enemyPos.x - center.x) + (enemyPos.y - center.y) * (enemyPos.y - center.y);
			if (dist2 <= POTION_RADIUS * POTION_RADIUS)
			{
				playerAddScore(player, enemyTakeDamage(deathSpawn->deaths[i], POTION_DAMAGE * playerGetMagicDamage(player)));
			}
		}
	}
}

void deathSpawnerUpdate(DeathSpawner* deathSpawn, uint32_t milliseconds)
{
	damageTick += (milliseconds / 1000.0f);
	if (damageTick >= 1.0f)
	{
		isHitting = !isHitting;
		damageTick = 0.0f;
	}

	// chance to spawn death
	timer += (milliseconds / 1000.0f);
	if (timer >= 10.0f)
	{
		if (deathSpawn->posCount > 0)
		{
			uint32_t chance = randGetInt(1, 100);
			if (chance <= deathSpawn->spawnChance)
			{
				uint32_t pos = 1;
				if (deathSpawn->posCount > 1)
					pos = randGetInt(1, deathSpawn->posCount);

				if (deathSpawn->deaths[pos - 1] == NULL)
				{
					uint8_t dir = randGetInt(0, DIR_COUNT);
					deathSpawn->deaths[pos - 1] = enemyNew(deathSpawn->spawnPos[pos - 1], 2);
					enemySetDirection(deathSpawn->deaths[pos - 1], dir);
					soundPlay(deathSound);
				}
			}
		}
		timer = 0.0f;
	}

	for (uint32_t i = 0; i < MAX_DEATHS; ++i)
	{
		if (deathSpawn->deaths[i] != NULL && enemyCheckDead(deathSpawn->deaths[i]))
		{
			enemyDelete(deathSpawn->deaths[i]);
			deathSpawn->deaths[i] = NULL;
		}
	}

	// calculate astar path
	pathRecalc += milliseconds;
	if (pathRecalc >= 240.0f)
	{
		for (uint32_t i = 0; i < MAX_DEATHS; ++i)
		{
			if (deathSpawn->deaths[i] != NULL)
			{
				enemyUpdatePath(deathSpawn->deaths[i]);
			}
		}
		pathRecalc = 0.0f;
	}
}