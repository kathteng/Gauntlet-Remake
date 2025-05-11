#include "WallTile.h"
#include "Tile.h"


/// @brief the direction of the wall that would be collided
typedef enum direction_t
{
	DIR_NONE,
	DIR_TOP,		// top of wall gets collided
	DIR_BOTTOM,		// bottom of wall gets collided
	DIR_LEFT,		// left of wall gets collided
	DIR_RIGHT,		// right of wall gets collided
	DIR_CENTER		// left and right of wall gets collided
} Direction;

typedef struct walltile_t
{
	Tile tile;

	Direction dir[2];
} WallTile;

static char WALL_SPRITE[] = "asset/levels/wall_sprite.png";

static void _wallUpdate(Object* obj, uint32_t milliseconds);
static void _wallDraw(Object* obj);
static ObjVtable _wallVtable = {
	_wallDraw,
	_wallUpdate
};

WallTile* wallTileNew(Coord2D pos, uint16_t _asciiChar, uint8_t _dir)
{
	WallTile* wall = malloc(sizeof(WallTile));
	if (wall != NULL)
	{
		wall->tile = *tileNew(pos, WALL_SPRITE, _asciiChar);
		objInit(&wall->tile.obj, &_wallVtable, pos, wall->tile.obj.size, wall->tile.obj.velocity);
		if (_dir == DIR_CENTER)
		{
			wall->dir[0] = DIR_LEFT;
			wall->dir[1] = DIR_RIGHT;
		}
		else
		{
			wall->dir[0] = _dir;
			wall->dir[1] = DIR_NONE;
		}
	}

	return wall;
}

void wallTileDelete(WallTile* wall)
{
	tileDelete(&wall->tile);
	objDeinit(&wall->tile.obj);

	//free(wall);
}

bool wallCheckCollision(Object* obj, WallTile* wall)
{
	float xBoundLeft = wall->tile.obj.position.x - wall->tile.obj.size.x / 2.1f;
	float xBoundRight = wall->tile.obj.position.x + wall->tile.obj.size.x / 2.1f;
	float yBoundTop = wall->tile.obj.position.y - wall->tile.obj.size.x / 2.1f;
	float yBoundBottom = wall->tile.obj.position.y + wall->tile.obj.size.x / 2.1f;

	float objXLeft = obj->position.x - obj->size.x / 2;
	float objXRight = obj->position.x + obj->size.x / 2;
	float objYTop = obj->position.y - obj->size.y / 2;
	float objYBottom = obj->position.y + obj->size.y / 2;

	if (objXRight > xBoundLeft && objXLeft < xBoundRight && objYTop < yBoundBottom && objYBottom > yBoundTop)
	{
		return true;
	}
	
	return false;
}

static void _wallDraw(Object* obj)
{
	WallTile* wall = (WallTile*)obj;
	tileDraw(&wall->tile.obj);
}

static void _wallUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);
}