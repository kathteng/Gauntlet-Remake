#include "Astar.h"
#include <stdio.h>


typedef struct node_t
{
	Node* parent;
	uint32_t x, y;  // grid position
	float f, g, h;	// f = g + h
	bool walkable;
	uint16_t type;		// tile type
} Node;

static Node* grid[LEVEL_SIZE][LEVEL_SIZE];
static Node* player1 = NULL;
static Node* player2 = NULL;
static int8_t directions[8][2] = {
	{0, -1}, {0, 1}, {-1, 0}, {1, 0},	// N, S, W, E
	{-1, -1}, {-1, 1}, {1, -1}, {1, 1}	// NE, SE, NW, SW
};

Node* nodeNew(Node* _parent, uint32_t _x, uint32_t _y, float _f, float _g, float _h, uint16_t _type)
{
	Node* node = malloc(sizeof(Node));
	if (node != NULL)
	{
		node->parent = _parent;
		node->x = _x;
		node->y = _y;
		node->f = _f;
		node->g = _g;
		node->h = _h;
		node->type = _type;

		if (node->type == ' ')
			node->walkable = true;
		else
			node->walkable = false;
	}

	return node;
}

void nodeDelete(Node* node)
{
	if (node->parent != NULL)
		nodeDelete(node->parent);

	if (node != NULL)
		free(node);
}

void nodeInitializePlayers(Object* p1, Object* p2)
{
	if (p1 != NULL)
	{
		player1 = nodeNew(NULL, (uint32_t)(p1->position.x / TILE_SIZE), (uint32_t)(p1->position.y / TILE_SIZE), 0.0f, 0.0f, 0.0f, ' ');
	}
	if (p2 != NULL)
	{
		player2 = nodeNew(NULL, (uint32_t)(p2->position.x / TILE_SIZE), (uint32_t)(p2->position.y / TILE_SIZE), 0.0f, 0.0f, 0.0f, ' ');
	}
}

void initializeGrid(Tile* tiles[LEVEL_SIZE][LEVEL_SIZE])
{
	for (uint32_t i = 0; i < LEVEL_SIZE; ++i)
	{
		for (uint32_t j = 0; j < LEVEL_SIZE; ++j)
		{
			grid[i][j] = nodeNew(NULL, j, i, 0.0f, 0.0f, 0.0f, tileGetType(tiles[i][j]));
			char buffer[64];
			sprintf_s(buffer, sizeof(buffer), "%d \0", grid[i][j]->walkable);
			OutputDebugStringA(buffer);
		}
		OutputDebugStringA("\n");
	}
}

Node* selectTarget(Node* enemy)
{
	float distToPlayer1 = 0.0f;
	float distToPlayer2 = 0.0f;

	if (player1 != NULL)
		distToPlayer1 = heuristic(enemy, player1);

	if (player2 != NULL)
		distToPlayer2 = heuristic(enemy, player2);

	if (distToPlayer1 <= distToPlayer2 || player2 == NULL)
		return player1;
	else
		return player2;
}

bool checkTileWalkable(uint32_t x, uint32_t y)
{
	if (grid[y][x]->walkable)
		return true;
	else
		return false;
}

/// @brief Diagonal distance heuristic
float heuristic(Node* a, Node* b)
{
	uint32_t dx = abs(a->x - b->x);
	uint32_t dy = abs(a->y - b->y);

	return STRAIGHT_COST * (dx + dy) + (DIAGONAL_COST - 2 * STRAIGHT_COST) * (float)fmin(dx, dy);
}

void reconstructPath(Node* node, Node* start, uint32_t* nextX, uint32_t* nextY)
{
	while (node->parent != NULL && node != start)
	{
		*nextX = node->x;
		*nextY = node->y;
		node = node->parent;
	}
}

Node* findInList(Node* list[], uint32_t size, Node* node)
{
	for (uint32_t i = 0; i < size; ++i)
	{
		if (list[i] == node)
			return list[i];
	}

	return NULL;
}

void removeFromList(Node* list[], uint32_t* size, Node* node)
{
	for (uint32_t i = 0; i < *size; ++i)
	{
		if (list[i] == node)
		{
			for (uint32_t j = i; j < *size; ++j)
			{
				list[j] = list[j + 1];
			}
			--(*size);
			break;
		}
	}
}

void updateObstacle(uint32_t x, uint32_t y, bool walkable)
{
	if (x < 33 && x >= 0 && y < 33 && y >= 0)
		grid[y][x]->walkable = walkable;
}

void updatePlayerPos(Object* p1, Object* p2)
{
	uint32_t x = 0;
	uint32_t y = 0;
	if (p1 != NULL)
	{
		x = (uint32_t)(p1->position.x / TILE_SIZE);
		y = (uint32_t)(p1->position.y / TILE_SIZE);

		if (abs(x - player1->x) > 1)
			player1->x = x;
		if (abs(y - player1->y) > 1)
			player1->y = y;
	}
	else
	{
		player1->x = x;
		player1->y = y;
	}
	if (p2 != NULL)
	{
		x = (uint32_t)(p2->position.x / TILE_SIZE);
		y = (uint32_t)(p2->position.y / TILE_SIZE);

		if (abs(x - player2->x) > 1)
			player2->x = x;
		if (abs(y - player2->y) > 1)
			player2->y = y;
	}
	else
	{
		player2->x = x;
		player2->y = y;
	}
}

void updateEnemyNodePos(Node* enemy, uint32_t x, uint32_t y)
{
	enemy->x = x;
	enemy->y = y;
}

bool aStar(Node* start, Node* goal, uint32_t* nextX, uint32_t* nextY)
{
	if (goal == NULL)
		return false;

	// initialize lists
	Node* openList[LEVEL_TILES]; // nodes to be checked
	uint32_t openListSize = 0;
	Node* closedList[LEVEL_TILES]; // nodes already checked
	uint32_t closedListSize = 0;

	openList[openListSize++] = start;

	// main loop
	while (openListSize > 0)
	{
		Node* current = openList[0];

		for (uint32_t i = 1; i < openListSize; ++i)
		{
			if (openList[i]->f < current->f)
			{
				current = openList[i];
			}
		}

		// check if goal is reached
		if (current->x == goal->x && current->y == goal->y)
		{
			reconstructPath(current, start, nextX, nextY);
			return true;
		}

		// move current node to closed list
		removeFromList(openList, &openListSize, current);
		closedList[closedListSize++] = current;

		// process neighbor nodes
		for (uint32_t i = 0; i < 8; ++i)
		{
			int32_t nx = current->x + directions[i][0];
			int32_t ny = current->y + directions[i][1];

			if (nx < 0 || ny < 0 || nx >= LEVEL_SIZE || ny >= LEVEL_SIZE || !grid[ny][nx]->walkable)
				continue;

			if (i >= 4)
			{
				if (!grid[ny][current->x]->walkable || !grid[current->y][nx]->walkable)
					continue;
			}

			Node* neightbor = grid[ny][nx];

			if (findInList(closedList, closedListSize, neightbor))
				continue;

			float cost = STRAIGHT_COST;
			if (i >= 4)
				cost = DIAGONAL_COST;

			float tempG = current->g + cost;

			if (!findInList(openList, openListSize, neightbor) || tempG < neightbor->g)
			{
				neightbor->g = tempG;
				neightbor->h = heuristic(neightbor, goal);
				neightbor->f = neightbor->g + neightbor->h;
				neightbor->parent = current;

				if (!findInList(openList, openListSize, neightbor))
					openList[openListSize++] = neightbor;
			}
		}
	}
	return false; // no path found
}