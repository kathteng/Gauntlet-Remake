#include <Windows.h>
#include <gl/GLU.h>
#include <assert.h>
#include "SOIL.h"

#include "defs.h"
#include "Projectile.h"
#include "Object.h"
#include "Level.h"



typedef struct projectile_t
{
	Object obj;

	bool isActive;
	uint8_t character;
	uint8_t direction;
	uint32_t damage;
	Coord2D prevPos;
	char* projectileSprite;
	GLuint projectileTexture;
} Projectile;

static char WARRIOR_SPRITE[] = "asset/characters/warrior_projectile_spritesheet.png";
static char VALKYRIE_SPRITE[] = "asset/characters/valkyrie_projectile_spritesheet.png";
static char WIZARD_SPRITE[] = "asset/characters/wizard_projectile_spritesheet.png";
static char ELF_SPRITE[] = "asset/characters/elf_projectile_spritesheet.png";

static Coord2D _projectileSetDirection(uint8_t dir);
static void _projectileUpdate(Object* obj, uint32_t milliseconds);
static void _projectileDraw(Object* obj);
static ObjVtable _projectileVtable = {
	_projectileDraw,
	_projectileUpdate
};

void projectileInitTextures(Projectile* projectile)
{
	if (projectile->projectileTexture == 0)
	{
		projectile->projectileTexture = SOIL_load_OGL_texture(projectile->projectileSprite, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(projectile->projectileTexture != 0);
	}
}

Projectile* projectileNew(uint32_t _character, uint32_t _direction, uint32_t _damage, Coord2D _pos)
{
	Projectile* projectile = malloc(sizeof(Projectile));

	if (projectile != NULL)
	{
		Coord2D velocity = _projectileSetDirection(_direction);
		Coord2D _size = { PROJECT_SIZE, PROJECT_SIZE };

		objInit(&projectile->obj, &_projectileVtable, _pos, _size, velocity);

		projectile->isActive = false;
		projectile->character = _character;
		projectile->direction = _direction;
		projectile->damage = _damage;
		projectile->prevPos = _pos;
		projectile->projectileTexture = 0;

		if (_character == 0) //Warrior
		{
			projectile->projectileSprite = WARRIOR_SPRITE;
		}
		else if (_character == 1) // Valkyrie
		{
			projectile->projectileSprite = VALKYRIE_SPRITE;
		}
		else if (_character == 2) // Wizard
		{
			projectile->projectileSprite = WIZARD_SPRITE;
		}
		else if (_character == 3) // Elf
		{
			projectile->projectileSprite = ELF_SPRITE;
		}
	}

	return projectile;
}

void projectileDelete(Projectile* projectile)
{
	objDeinit(&projectile->obj);

	free(projectile);
}

bool projectileCheckActive(Projectile* projectile)
{
	return projectile->isActive;
}

void projectileDeactivate(Projectile* projectile)
{
	projectile->isActive = false;
}

void projectileSpawn(Projectile* projectile, uint8_t dir, Coord2D pos)
{
	projectile->isActive = true;
	projectile->direction = dir;
	projectile->obj.position = pos;
	projectile->prevPos = pos;
	projectile->obj.velocity = _projectileSetDirection(dir);
}

uint32_t projectileGetDamage(Projectile* projectile)
{
	return projectile->damage;
}

static Coord2D _projectileSetDirection(uint8_t dir)
{
	Coord2D velocity = { 0.0f, 0.0f };

	if (dir == 0) // south
	{
		velocity.x = 0.0f;
		velocity.y = PROJECT_SPEED;
	}
	else if (dir == 1) // west
	{
		velocity.x = -PROJECT_SPEED;
		velocity.y = 0.0f;
	}
	else if (dir == 2) // north
	{
		velocity.x = 0.0f;
		velocity.y = -PROJECT_SPEED;
	}
	else if (dir == 3) // east
	{
		velocity.x = PROJECT_SPEED;
		velocity.y = 0.0f;
	}
	else if (dir == 4) // southwest
	{
		velocity.x = -PROJECT_SPEED;
		velocity.y = PROJECT_SPEED;
	}
	else if (dir == 5) // northwest
	{
		velocity.x = -PROJECT_SPEED;
		velocity.y = -PROJECT_SPEED;
	}
	else if (dir == 6) // northeast
	{
		velocity.x = PROJECT_SPEED;
		velocity.y = -PROJECT_SPEED;
	}
	else if (dir == 7) // southeast
	{
		velocity.x = PROJECT_SPEED;
		velocity.y = PROJECT_SPEED;
	}

	return velocity;
}

static void _projectileDraw(Object* obj)
{
	Projectile* projectile = (Projectile*)obj;

	if (projectile->isActive)
	{
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, projectile->projectileTexture);
		// disable anti-aliasing
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = (obj->position.x - projectile->obj.size.x / 2);
			GLfloat xPosRight = (obj->position.x + projectile->obj.size.x / 2);
			GLfloat yPosTop = (obj->position.y - projectile->obj.size.y / 2);
			GLfloat yPosBottom = (obj->position.y + projectile->obj.size.y / 2);

			float uPerChar = 1.0f / DIR_COUNT;

			GLfloat xTexCoord = projectile->direction * uPerChar;

			const float BG_DEPTH = 0.95f;

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

static void _projectileUpdate(Object* obj, uint32_t milliseconds)
{
	Projectile* projectile = (Projectile*)obj;
	projectile->prevPos = projectile->obj.position;
	
	if (checkWallsCollision((Object*)projectile) || checkDoorsCollision((Object*)projectile))
	{
		projectile->isActive = false;
	}

	objDefaultUpdate(obj, milliseconds);

	/*if (obj->position.x >= SCREEN_WIDTH || obj->position.x < 0 || obj->position.y >= SCREEN_HEIGHT || obj->position.y < 0)
	{
		projectile->isActive = false;
	}*/
}