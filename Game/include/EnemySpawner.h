#pragma once
#include "baseTypes.h"
#include "Object.h"
#include "Player.h"


typedef struct enemyspawner_t EnemySpawner;

void enemySpawnerInitTextures(EnemySpawner* enemySpawn);
EnemySpawner* enemySpawnerNew(Coord2D pos, uint8_t type);
void enemySpawnerDelete(EnemySpawner* enemySpawn);
void spawnEnemies(EnemySpawner* enemySpawn);

void enemiesCheckCollisionPlayer(EnemySpawner* enemySpawn, Object* player);
void enemiesCheckCollisionProjectile(EnemySpawner* enemySpawn, Object* projectile, Object* player);
void enemiesCheckPotionRadius(EnemySpawner* enemySpawn, Coord2D center, Player* player);
bool enemySpawnerCheckCollision(EnemySpawner* enemySpawn, Object* obj);
bool enemySpawnerCheckDestroyed(EnemySpawner* enemySpawn);
uint32_t enemySpawnerTakeDamage(EnemySpawner* enemySpawn);