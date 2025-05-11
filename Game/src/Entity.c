#include <stdlib.h>
#include "Entity.h"


Entity* entityNew(uint32_t _maxHealth)
{
	Entity* entity = malloc(sizeof(Entity));
	if (entity != NULL)
	{
		entity->currDir = SOUTH;
		entity->maxHealth = _maxHealth;
		entity->currHealth = _maxHealth;
	}

	return entity;
}

void entityDelete(Entity* entity)
{
	free(entity);
}
