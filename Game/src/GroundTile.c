#include "GroundTile.h"
#include "Tile.h"


typedef struct groundtile_t
{
	Tile tile;
} GroundTile;

static char GROUND_SPRITE[] = "asset/levels/ground_sprite.png";

static void _groundUpdate(Object* obj, uint32_t milliseconds);
static void _groundDraw(Object* obj);
static ObjVtable _groundVtable = {
	_groundDraw,
	_groundUpdate
};

GroundTile* groundTileNew(Coord2D pos, uint16_t _asciiChar)
{
	GroundTile* ground = malloc(sizeof(GroundTile));
	if (ground != NULL)
	{
		ground->tile = *tileNew(pos, GROUND_SPRITE, _asciiChar);
		objInit(&ground->tile.obj, &_groundVtable, pos, ground->tile.obj.size, ground->tile.obj.velocity);
	}

	return ground;
}

void groundTileDelete(GroundTile* ground)
{
	tileDelete(&ground->tile);
	objDeinit(&ground->tile.obj);

	//free(ground);
}

static void _groundDraw(Object* obj)
{
	GroundTile* ground = (GroundTile*)obj;
	tileDraw(&ground->tile.obj);
}

static void _groundUpdate(Object* obj, uint32_t milliseconds)
{
	objDefaultUpdate(obj, milliseconds);
}