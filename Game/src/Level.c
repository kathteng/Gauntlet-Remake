#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "Level.h"
#include "Tile.h"
#include "GroundTile.h"
#include "ExitTile.h"
#include "EnemySpawner.h"
#include "DeathSpawner.h"
#include "Key.h"
#include "Treasure.h"
#include "Food.h"
#include "Potion.h"
#include "Astar.h"
#include "sound.h"


typedef struct level_t
{
	uint8_t levelNum;
	Tile* tiles[LEVEL_SIZE][LEVEL_SIZE];
	GroundTile* grounds[LEVEL_TILES];
	ExitTile* exits[MAX_EXITS];
	EnemySpawner* enemySpawners[MAX_ENEMY_SPAWNERS];
	DeathSpawner* deathSpawner;

	Coord2D player1SpawnPos;
	Coord2D player2SpawnPos;
	Key* key[MAX_ITEMS];
	Treasure* treasure[MAX_ITEMS];
	Food* food[MAX_ITEMS];
	Potion* potion[MAX_ITEMS];
} Level;

static char levelFile[] = "asset/levels/level1.txt";

static void levelLoad(Level* level);

Level* levelNew(uint8_t num)
{
	Level* level = malloc(sizeof(Level));
	if (level != NULL)
	{
		Coord2D start = { 0.0f, 0.0f };
		level->player1SpawnPos = start;
		level->player2SpawnPos = start;
		level->levelNum = num;
		level->deathSpawner = deathSpawnerNew(num * 10);

		for (uint32_t i = 0; i < LEVEL_TILES; ++i)
		{
			walls[i] = NULL;
			doors[i] = NULL;
			level->grounds[i] = NULL;
		}

		for (uint32_t i = 0; i < MAX_ITEMS; ++i)
		{
			level->key[i] = NULL;
			level->treasure[i] = NULL;
			level->food[i] = NULL;
			level->potion[i] = NULL;
		}

		for (uint32_t i = 0; i < MAX_EXITS; ++i)
		{
			level->exits[i] = NULL;
		}

		for (uint32_t i = 0; i < MAX_ENEMY_SPAWNERS; ++i)
		{
			level->enemySpawners[i] = NULL;
		}
		levelLoad(level);
	}
	return level;
}

void levelDelete(Level* level)
{
	if (level != NULL)
	{
		deathSpawnerDelete(level->deathSpawner);

		for (uint32_t i = 0; i < LEVEL_TILES; ++i)
		{
			if (walls[i] != NULL)
			{
				wallTileDelete(walls[i]);
				walls[i] = NULL;
			}
			if (doors[i] != NULL)
			{
				doorDelete(doors[i]);
				doors[i] = NULL;
			}
			if (level->grounds[i] != NULL)
			{
				groundTileDelete(level->grounds[i]);
				level->grounds[i] = NULL;
			}
		}

		for (uint32_t i = 0; i < MAX_EXITS; ++i)
		{
			if (level->exits[i] != NULL)
			{
				exitTileDelete(level->exits[i]);
				level->exits[i] = NULL;
			}
		}
		
		for (uint32_t i = 0; i < MAX_ENEMY_SPAWNERS; ++i)
		{
			if (level->enemySpawners[i] != NULL)
			{
				enemySpawnerDelete(level->enemySpawners[i]);
				level->enemySpawners[i] = NULL;
			}
		}

		for (uint32_t i = 0; i < MAX_ITEMS; ++i)
		{
			if (level->key[i] != NULL)
			{
				keyDelete(level->key[i]);
				level->key[i] = NULL;
			}
			if (level->treasure[i] != NULL)
			{
				treasureDelete(level->treasure[i]);
				level->treasure[i] = NULL;
			}
			if (level->food[i] != NULL)
			{
				foodDelete(level->food[i]);
				level->food[i] = NULL;
			}
			if (level->potion[i] != NULL)
			{
				potionDelete(level->potion[i]);
				level->potion[i] = NULL;
			}
		}
	}
	free(level);
}

static void levelLoad(Level* level)
{
	FILE* fileptr = NULL;
	errno_t err;
	levelFile[18] = level->levelNum + '0';
	err = fopen_s(&fileptr, levelFile, "r");

	if (err == 0)
	{
		uint32_t i = 0;
		uint32_t j = 0;
		uint32_t w = 0; // wall counter
		uint32_t d = 0; // door counter
		uint32_t g = 0; // ground counter
		uint32_t x = 0; // exit counter
		uint32_t e = 0; // enemy spawner counter
		uint32_t k = 0; // key counter
		uint32_t t = 0; // treasure counter
		uint32_t p = 0; // potion counter
		uint32_t f = 0; // food counter
		while (true)
		{
			char c = fgetc(fileptr);

			if (feof(fileptr))
				break;

			Coord2D pos = { TILE_SIZE / 2 + TILE_SIZE * j, TILE_SIZE / 2 + TILE_SIZE * i};

			if (c == WALL) // wall tile
			{
				uint8_t dir = 0;
				if (i == 0)
					dir = 1;
				else if (i == 32)
					dir = 2;
				else if (j == 0)
					dir = 4;
				else if (j == 32)
					dir = 3;
				else
					dir = 5;

				WallTile* wall = wallTileNew(pos, c, dir);
				walls[w] = wall;
				level->tiles[i][j] = (Tile*)wall;
				++w;
			}
			else if (c == GROUND) // ground tile
			{
				GroundTile* ground = groundTileNew(pos, c);
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;
			}
			else if (c == DOOR_V || c == DOOR_H) // door tile
			{
				uint8_t dir = 0;
				if (c == DOOR_V)
					dir = 1;
				
				Door* door = doorNew(pos, dir);
				doors[d] = door;
				GroundTile* ground = groundTileNew(pos, c);
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++d;
				++g;
			}
			else if (c == KEY) // key spawn tile
			{
				level->key[k] = keyNew(pos);
				++k;
				GroundTile* ground = groundTileNew(pos, c);
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;
			}
			else if (c == TREASURE) // treasure spawn tile
			{
				level->treasure[t] = treasureNew(pos, 100);
				++t;
				GroundTile* ground = groundTileNew(pos, c);
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;
			}
			else if (c == FOOD) // food spawn tile
			{
				level->food[f] = foodNew(pos, 100);
				++f;
				GroundTile* ground = groundTileNew(pos, c);
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;
			}
			else if (c == POTION) // potion spawn tile
			{
				level->potion[p] = potionNew(pos, POTION_DAMAGE);
				++p;
				GroundTile* ground = groundTileNew(pos, c);
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;
			}
			else if (c == GHOST_SPAWN) // ghost spawner tile
			{
				GroundTile* ground = groundTileNew(pos, c);
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;

				level->enemySpawners[e] = enemySpawnerNew(pos, 0);
				enemySpawnerInitTextures(level->enemySpawners[e]);
				++e;
			}
			else if (c == GRUNT_SPAWN) // grunt spawner tile
			{
				GroundTile* ground = groundTileNew(pos, c);
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;

				level->enemySpawners[e] = enemySpawnerNew(pos, 1);
				enemySpawnerInitTextures(level->enemySpawners[e]);
				++e;
			}
			else if (c == DEATH_SPAWN) // death potential spawn pos
			{
				GroundTile* ground = groundTileNew(pos, c);
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;

				deathSpawnerAddPos(level->deathSpawner, pos);
			}
			else if (c == P1_POS)
			{
				GroundTile* ground = groundTileNew(pos, ' ');
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;

				level->player1SpawnPos = pos;
			}
			else if (c == P2_POS)
			{
				GroundTile* ground = groundTileNew(pos, ' ');
				level->grounds[g] = ground;
				level->tiles[i][j] = (Tile*)ground;
				++g;

				level->player2SpawnPos = pos;
			}
			else if (c == EXIT) // exit tile
			{
				ExitTile* exit = exitTileNew(pos, c, level->levelNum + 1);
				level->exits[x] = exit;
				level->tiles[i][j] = (Tile*)exit;
				++x;
			}
			else
				continue;

			++j;
			if (j == LEVEL_SIZE)
			{
				++i;
				j = 0;
			}
		}

		fclose(fileptr);
	}
	// initialize node grid for A*
	initializeGrid(level->tiles);

	for (uint32_t i = 0; i < MAX_ENEMY_SPAWNERS; ++i)
	{
		if (level->enemySpawners[i] != NULL)
			spawnEnemies(level->enemySpawners[i]);
	}
}

void levelSetPlayerSpawns(Level* level, Object* p1, Object* p2)
{
	p1->position = level->player1SpawnPos;
	if (p2 != NULL)
		p2->position = level->player2SpawnPos;
}

bool checkWallsCollision(Object* obj)
{
	for (uint32_t i = 0; i < LEVEL_TILES; ++i)
	{
		if (walls[i] != NULL && wallCheckCollision(obj, walls[i]))
		{
			return true;
		}
	}

	return false;
}

bool checkDoorsCollision(Object* obj)
{
	for (uint32_t i = 0; i < LEVEL_TILES; ++i)
	{
		if (doors[i] != NULL && doorCheckCollision(obj, doors[i]))
		{
			return true;
		}
	}

	return false;
}

uint8_t checkExitsCollision(Level* level, Object* player)
{
	for (uint32_t i = 0; i < MAX_EXITS; ++i)
	{
		if (level->exits[i] != NULL)
		{
			if (exitCheckCollision(level->exits[i], player))
			{
				return exitGetNextLevel(level->exits[i]);
			}
		}
	}
	return 0;
}

bool checkSpawnersCollisionPlayer(Level* level, Object* obj)
{
	for (uint32_t i = 0; i < MAX_ENEMY_SPAWNERS; ++i)
	{
		if (level->enemySpawners[i] != NULL)
		{
			if (enemySpawnerCheckCollision(level->enemySpawners[i], obj))
			{
				return true;
			}
		}
	}

	return false;
}

uint32_t checkSpawnersCollisionProjectile(Level* level, Projectile* projectile)
{
	for (uint32_t i = 0; i < MAX_ENEMY_SPAWNERS; ++i)
	{
		if (level->enemySpawners[i] != NULL)
		{
			if (enemySpawnerCheckCollision(level->enemySpawners[i], (Object*)projectile))
			{
				if (projectileCheckActive(projectile))
				{
					projectileDeactivate(projectile);
					return enemySpawnerTakeDamage(level->enemySpawners[i]);
				}
			}
		}
	}

	return 0;
}

void checkEnemiesCollision(Level* level, Object* player, Object* projectile)
{
	for (uint32_t i = 0; i < MAX_ENEMY_SPAWNERS; ++i)
	{
		if (level->enemySpawners[i] != NULL)
		{
			enemiesCheckCollisionPlayer(level->enemySpawners[i], player);
			enemiesCheckCollisionProjectile(level->enemySpawners[i], projectile, player);
		}
	}
	/// TODO: fix projectile check (can't do casting check)
	
	deathCheckCollisionPlayer(level->deathSpawner, player);
	deathCheckCollisionProjectile(level->deathSpawner, projectile);
}

bool checkKeyCollect(Level* level, Player* player1, Player* player2)
{
	for (uint32_t i = 0; i < MAX_ITEMS; ++i)
	{
		if (level->key[i] != NULL)
		{
			if (player1 != NULL && keyCheckCollision(level->key[i], (Object*)player1))
			{
				keySetPickedUp(level->key[i]);
				playerSetKeyCount(player1);
				if (player2 != NULL)
				{
					playerSetKeyCount(player2);
				}

				Coord2D pos = keyGetPos(level->key[i]);
				updateObstacle((uint32_t)(pos.x / TILE_SIZE), (uint32_t)(pos.y / TILE_SIZE), true);
				return true;
			}
			else if (player2 != NULL && keyCheckCollision(level->key[i], (Object*)player2))
			{
				keySetPickedUp(level->key[i]);
				playerSetKeyCount(player1);
				playerSetKeyCount(player2);

				Coord2D pos = keyGetPos(level->key[i]);
				updateObstacle((uint32_t)(pos.x / TILE_SIZE), (uint32_t)(pos.y / TILE_SIZE), true);
				return true;
			}
		}
	}
	return false;
}

bool checkTreasureCollect(Level* level, Player* player)
{
	for (uint32_t i = 0; i < MAX_ITEMS; ++i)
	{
		if (level->treasure[i] != NULL)
		{
			if (treasureCheckCollision(level->treasure[i], (Object*)player))
			{
				playerAddScore(player, treasureGetPoints(level->treasure[i]));
				
				Coord2D pos = treasureGetPos(level->treasure[i]);
				updateObstacle((uint32_t)(pos.x / TILE_SIZE), (uint32_t)(pos.y / TILE_SIZE), true);

				treasureDelete(level->treasure[i]);
				level->treasure[i] = NULL;
				return true;
			}
		}
	}
	return false;
}

bool checkFoodCollect(Level* level, Player* player)
{
	for (uint32_t i = 0; i < MAX_ITEMS; ++i)
	{
		if (level->food[i] != NULL)
		{
			if (foodCheckCollision(level->food[i], (Object*)player))
			{
				playerAddHealth(player, foodGetHealth(level->food[i]));

				Coord2D pos = foodGetPos(level->food[i]);
				updateObstacle((uint32_t)(pos.x / TILE_SIZE), (uint32_t)(pos.y / TILE_SIZE), true);

				foodDelete(level->food[i]);
				level->food[i] = NULL;
				return true;
			}
		}
	}
	return false;
}

bool checkPotionCollect(Level* level, Player* player)
{
	for (uint32_t i = 0; i < MAX_ITEMS; ++i)
	{
		if (level->potion[i] != NULL)
		{
			if (potionCheckCollision(level->potion[i], (Object*)player))
			{
				potionSetCollected(level->potion[i]);
				playerSetPotionCount(player);

				Coord2D pos = potionGetPos(level->potion[i]);
				updateObstacle((uint32_t)(pos.x / TILE_SIZE), (uint32_t)(pos.y / TILE_SIZE), true);
				
				potionDelete(level->potion[i]);
				level->potion[i] = NULL;
				return true;
			}
		}
	}
	return false;
}

void levelOpenDoors(Coord2D playerPos)
{
	uint32_t x = (uint32_t)(playerPos.x / TILE_SIZE);
	uint32_t y = (uint32_t)(playerPos.y / TILE_SIZE);

	for (uint32_t i = 0; i < LEVEL_TILES; ++i)
	{
		if (doors[i] != NULL)
		{
			Coord2D pos = doorGetPos(doors[i]);
			uint32_t doorX = (uint32_t)(pos.x / TILE_SIZE);
			uint32_t doorY = (uint32_t)(pos.y / TILE_SIZE);

			if (doorX == x + 1 || doorX == x - 1 || doorY == y + 1 || doorY == y - 1)
			{
				doorSetOpened(doors[i]);
				updateObstacle(doorX, doorY, true);
				x = doorX;
				y = doorY;
			}
		}
	}
}

void levelUsePotion(Level* level, Player* player)
{
	Coord2D center = playerGetPos(player);

	for (uint32_t i = 0; i < MAX_ENEMY_SPAWNERS; ++i)
	{
		if (level->enemySpawners[i] != NULL)
		{
			enemiesCheckPotionRadius(level->enemySpawners[i], center, player);
		}
	}

	deathCheckPotionRadius(level->deathSpawner, center, player);
}

void levelDraw(Level* level)
{
	for (uint32_t i = 0; i < LEVEL_SIZE; ++i)
	{
		for (uint32_t j = 0; j < LEVEL_SIZE; ++j)
		{
			Object* obj = (Object*)level->tiles[i][j];
			if (obj != NULL)
			{
				objDraw(obj);
			}
		}
	}
}

void levelUpdate(Level* level, uint32_t milliseconds)
{
	// delete destroyed spawners
	for (uint32_t i = 0; i < MAX_ENEMY_SPAWNERS; ++i)
	{
		if (level->enemySpawners[i] != NULL)
		{
			if (enemySpawnerCheckDestroyed(level->enemySpawners[i]))
			{
				enemySpawnerDelete(level->enemySpawners[i]);
				level->enemySpawners[i] = NULL;
			}
		}
	}

	// delete opened doors
	for (uint32_t i = 0; i < LEVEL_TILES; ++i)
	{
		if (doors[i] != NULL && doorCheckOpened(doors[i]))
		{
			doorDelete(doors[i]);
			doors[i] = NULL;
		}
	}

	deathSpawnerUpdate(level->deathSpawner, milliseconds);
}