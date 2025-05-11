#include "ExitTile.h"
#include "Tile.h"


typedef struct exittile_t
{
	Tile tile;

	uint8_t nextLevel;
} ExitTile;

static char EXIT_SPRITE[] = "asset/levels/exit_sprite.png";

static void _exitUpdate(Object* obj, uint32_t milliseconds);
static void _exitDraw(Object* obj);
static ObjVtable _exitVtable = {
	_exitDraw,
	_exitUpdate
};

ExitTile* exitTileNew(Coord2D pos, uint16_t _asciiChar, uint8_t level)
{
	ExitTile* exit = malloc(sizeof(ExitTile));
	if (exit != NULL)
	{
		exit->tile = *tileNew(pos, EXIT_SPRITE, _asciiChar);
		objInit(&exit->tile.obj, &_exitVtable, pos, exit->tile.obj.size, exit->tile.obj.velocity);
		exit->nextLevel = level;
	}

	return exit;
}

void exitTileDelete(ExitTile* exit)
{
	tileDelete(&exit->tile);
	objDeinit(&exit->tile.obj);

	//free(exit);
}

bool exitCheckCollision(ExitTile* exit, Object* player)
{
	float xBoundLeft = exit->tile.obj.position.x - exit->tile.obj.size.x / 3.0f;
	float xBoundRight = exit->tile.obj.position.x + exit->tile.obj.size.x / 3.0f;
	float yBoundTop = exit->tile.obj.position.y - exit->tile.obj.size.x / 3.0f;
	float yBoundBottom = exit->tile.obj.position.y + exit->tile.obj.size.x / 3.0f;

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

uint8_t exitGetNextLevel(ExitTile* exit)
{
	return exit->nextLevel;
}

static void _exitDraw(Object* obj)
{
	ExitTile* exit = (ExitTile*)obj;
	tileDraw(&exit->tile.obj);
}

static void _exitUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);
}