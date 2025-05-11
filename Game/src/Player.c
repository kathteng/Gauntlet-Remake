#include <Windows.h>
#include <gl/GLU.h>
#include <assert.h>
#include "SOIL.h"

#include "Player.h"
#include "defs.h"
#include "Entity.h"
#include "Object.h"
#include "Potion.h"



typedef struct player_t
{
	Entity entity;

	PlayerCharacter character;
	PlayerAnimState state;
	Projectile* projectile;
	Coord2D prevPos;
	uint32_t meleeDamage;
	uint32_t magicDamage;
	uint32_t armor;
	float speed;
	float frameTimer;
	uint8_t currFrame;
	GLuint playerTexture;
	char* playerSprite;

	uint32_t keyCount;
	uint32_t potionCount;
	Potion* potion;
	uint32_t score;
} Player;

static char WARRIOR_SPRITE[] = "asset/characters/warrior_spritesheet.png";
static char VALKYRIE_SPRITE[] = "asset/characters/valkyrie_spritesheet.png";
static char WIZARD_SPRITE[] = "asset/characters/wizard_spritesheet.png";
static char ELF_SPRITE[] = "asset/characters/elf_spritesheet.png";

static void _playerUpdate(Object* obj, uint32_t milliseconds);
static void _playerDraw(Object* obj);
static ObjVtable _playerVtable = {
	_playerDraw,
	_playerUpdate
};

void playerInitTextures(Player* player)
{
	if (player->playerTexture == 0)
	{
		player->playerTexture = SOIL_load_OGL_texture(player->playerSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(player->playerTexture != 0);
	}
}

Player* playerNew(uint8_t _character, Coord2D _pos)
{
	Player* player = malloc(sizeof(Player));
	if (player != NULL)
	{
		Coord2D velocity = { 0.0f, 0.0f };
		Coord2D size = { TILE_SIZE, TILE_SIZE };
		uint32_t maxHealth = PLAYER_MAX_HEALTH;

		if (_character == WARRIOR)
		{
			player->playerSprite = WARRIOR_SPRITE;
			player->meleeDamage = 20;
			player->magicDamage = 15;
			player->armor = 1;
			player->speed = 0.15f;
		}
		else if (_character == VALKYRIE)
		{
			player->playerSprite = VALKYRIE_SPRITE;
			player->meleeDamage = 15;
			player->magicDamage = 15;
			player->armor = 2;
			player->speed = 0.15f;
		}
		else if (_character == WIZARD)
		{
			player->playerSprite = WIZARD_SPRITE;
			player->meleeDamage = 15;
			player->magicDamage = 20;
			player->armor = 1;
			player->speed = 0.15f;
		}
		else if (_character == ELF)
		{
			player->playerSprite = ELF_SPRITE;
			player->meleeDamage = 15;
			player->magicDamage = 15;
			player->armor = 1;
			player->speed = 0.2f;
		}
		player->entity = *entityNew(maxHealth);
		objInit(&player->entity.obj, &_playerVtable, _pos, size, velocity);

		player->potion = NULL;
		player->character = _character;
		player->state = IDLE;
		player->prevPos = _pos;
		player->frameTimer = 0.0f;
		player->currFrame = 0;
		player->playerTexture = 0;

		player->keyCount = 0;
		player->potionCount = 0;
		player->score = 0;
		player->projectile = projectileNew(player->character, player->entity.currDir, player->meleeDamage, player->entity.obj.position);
		playerInitTextures(player);
		projectileInitTextures(player->projectile);
	}

	return player;
}

void playerDelete(Player* player)
{
	if (player->potion != NULL)
		potionDelete(player->potion);

	projectileDelete(player->projectile);
	entityDelete(&player->entity);
	objDeinit(&player->entity.obj);

	//free(player);
}

void playerFireProjectile(Player* player)
{
	if (!projectileCheckActive(player->projectile))
	{
		projectileSpawn(player->projectile, player->entity.currDir, player->entity.obj.position);
	}
}

void playerSetDirection(Player* player, uint8_t dir)
{
	player->entity.currDir = dir;
}

void playerSetVelocity(Player* player, float _x, float _y)
{
	player->entity.obj.velocity.x = player->speed * _x;
	player->entity.obj.velocity.y = player->speed * _y;
}

void playerSetKeyCount(Player* player)
{
	++player->keyCount;
}

void playerUseKey(Player* player)
{
	--player->keyCount;
}

void playerSetPotionCount(Player* player)
{
	if (player->potionCount == 0 && player->potion == NULL)
	{
		Coord2D pos = { -100.0f, -100.0f };
		player->potion = potionNew(pos, POTION_DAMAGE);
	}
	++player->potionCount;
}

void playerUsePotion(Player* player)
{
	--player->potionCount;
}

void playerResetPos(Player* player)
{
	player->entity.obj.position = player->prevPos;
}

void playerAddScore(Player* player, uint32_t points)
{
	player->score += points;
	if (player->score > 99999)
		player->score = 99999;
}

void playerAddHealth(Player* player, uint32_t health)
{
	player->entity.currHealth += health;
	if (player->entity.currHealth > 9999)
		player->entity.currHealth = 9999;
}

void playerTakeDamage(Player* player, uint32_t damage)
{
	damage = damage / player->armor;

	player->entity.currHealth -= damage;
	if (player->entity.currHealth < 0)
		player->entity.currHealth = 0;
}

void playerSetState(Player* player, uint8_t state)
{
	player->state = state;
}

uint8_t playerGetState(Player* player)
{
	return player->state;
}

uint8_t playerGetCharacter(Player* player)
{
	return player->character;
}

uint32_t playerGetMagicDamage(Player* player)
{
	return player->magicDamage;
}

uint32_t playerGetKeyCount(Player* player)
{
	return player->keyCount;
}

uint32_t playerGetPotionCount(Player* player)
{
	return player->potionCount;
}

int32_t playerGetHealth(Player* player)
{
	return player->entity.currHealth;
}

uint32_t playerGetScore(Player* player)
{
	return player->score;
}

Coord2D playerGetPos(Player* player)
{
	return player->entity.obj.position;
}

Coord2D playerGetPosDiff(Player* player)
{
	Coord2D diff = { 0.0f, 0.0f };

	if (player->entity.obj.position.x < TILES_ON_SCREEN * TILE_SIZE && player->entity.obj.position.y < TILES_ON_SCREEN * TILE_SIZE)
		return diff;

	// moving down more
	if (player->entity.obj.position.x < TILES_ON_SCREEN * TILE_SIZE && player->entity.obj.position.y >= TILES_ON_SCREEN * TILE_SIZE)
	{
		diff.x = 0.0f;
		diff.y = player->entity.obj.position.y - player->prevPos.y;
	}
	// moving right more
	else if (player->entity.obj.position.x >= TILES_ON_SCREEN * TILE_SIZE && player->entity.obj.position.y < TILES_ON_SCREEN * TILE_SIZE)
	{
		diff.x = player->entity.obj.position.x - player->prevPos.x;
		diff.y = 0.0f;
	}
	else
	{
		diff.x = player->entity.obj.position.x - player->prevPos.x;
		diff.y = player->entity.obj.position.y - player->prevPos.y;
	}
	return diff;
}

uint32_t playerGetMeleeDamage(Player* player)
{
	return player->meleeDamage;
}

Projectile* playerGetProjectile(Player* player)
{
	return player->projectile;
}

static void _playerDraw(Object* obj)
{
	Player* player = (Player*)obj;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, player->playerTexture);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = (obj->position.x - player->entity.obj.size.x / 2);
		GLfloat xPosRight = (obj->position.x + player->entity.obj.size.x / 2);
		GLfloat yPosTop = (obj->position.y - player->entity.obj.size.y / 2);
		GLfloat yPosBottom = (obj->position.y + player->entity.obj.size.y / 2);
		
		float uPerChar = 1.0f / DIR_COUNT;
		float vPerChar = 1.0f / 5.0f;

		GLfloat xTexCoord = player->entity.currDir * uPerChar;
		GLfloat yTexCoord = player->currFrame * vPerChar;

		if (player->state == IDLE)
		{
			yTexCoord = 3.0f * vPerChar;
		}
		else if (player->state == WALK)
		{
			yTexCoord = (player->currFrame + 2) * vPerChar;
		}

		const float BG_DEPTH = 0.95f;

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

static void _playerUpdate(Object* obj, uint32_t milliseconds)
{
	Player* player = (Player*)obj;
	player->prevPos = player->entity.obj.position;

	player->frameTimer += milliseconds;
	if (player->frameTimer >= ANIM_FRAME_RATE)
	{
		if (player->state != IDLE)
		{
			++player->currFrame;
			if (player->state == WALK && player->currFrame == 3)
				player->currFrame = 0;
			else if (player->state == ATTACK && player->currFrame == 2)
				player->currFrame = 0;
		}
		else
			player->currFrame = 0;
		player->frameTimer = 0.0f;
	}

	objDefaultUpdate(obj, milliseconds);
}