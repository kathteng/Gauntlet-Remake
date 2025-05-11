#pragma once
#include "baseTypes.h"
#include "Projectile.h"


typedef enum playercharacter_t PlayerCharacter;
typedef enum playeranimstate_t PlayerAnimState;
typedef struct player_t Player;

typedef enum playercharacter_t
{
	WARRIOR,
	VALKYRIE,
	WIZARD,
	ELF
} PlayerCharacter;

typedef enum playeranimstate_t
{
	IDLE,
	WALK,
	ATTACK,
	ANIM_COUNT
} PlayerAnimState;

void playerInitTextures(Player* player);
Player* playerNew(uint8_t _character, Coord2D _pos);
void playerDelete(Player* player);

void playerFireProjectile(Player* player);
void playerSetDirection(Player* player, uint8_t dir);
void playerSetVelocity(Player* player, float _x, float _y);
void playerSetKeyCount(Player* player);
void playerUseKey(Player* player);
void playerSetPotionCount(Player* player);
void playerUsePotion(Player* player);

void playerResetPos(Player* player);
void playerAddScore(Player* player, uint32_t points);
void playerAddHealth(Player* player, uint32_t health);
void playerTakeDamage(Player* player, uint32_t damage);
void playerSetState(Player* player, uint8_t state);

uint8_t playerGetState(Player* player);
uint8_t playerGetCharacter(Player* player);
uint32_t playerGetMagicDamage(Player* player);
uint32_t playerGetKeyCount(Player* player);
uint32_t playerGetPotionCount(Player* player);
int32_t playerGetHealth(Player* player);
uint32_t playerGetScore(Player* player);
Coord2D playerGetPos(Player* player);
Coord2D playerGetPosDiff(Player* player);
uint32_t playerGetMeleeDamage(Player* player);
Projectile* playerGetProjectile(Player* player);