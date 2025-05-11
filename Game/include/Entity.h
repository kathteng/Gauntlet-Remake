#pragma once
#include "baseTypes.h"
#include "Object.h"



typedef enum entitydir_t EntityDir;
typedef struct entity_t Entity;

typedef enum entitydir_t
{
	SOUTH,
	WEST,
	NORTH,
	EAST,
	SOUTHWEST,
	NORTHWEST,
	NORTHEAST,
	SOUTHEAST
} EntityDir;

typedef struct entity_t
{
	Object obj;

	EntityDir currDir;
	uint32_t maxHealth;
	int32_t currHealth;
} Entity;

Entity* entityNew(uint32_t _maxHealth);
void entityDelete(Entity* entity);
