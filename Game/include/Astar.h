#pragma once
#include "baseTypes.h"
#include "defs.h"
#include "Tile.h"


typedef struct node_t Node;

Node* nodeNew(Node* parent, uint32_t x, uint32_t y, float f, float g, float h, uint16_t type);
void nodeDelete(Node* node);

void nodeInitializePlayers(Object* p1, Object* p2);
void initializeGrid(Tile* tiles[LEVEL_SIZE][LEVEL_SIZE]);
Node* selectTarget(Node* enemy);
bool checkTileWalkable(uint32_t x, uint32_t y);

float heuristic(Node* a, Node* b);
void reconstructPath(Node* node, Node* start, uint32_t* nextX, uint32_t* nextY);
Node* findInList(Node* list[], uint32_t size, Node* node);
void removeFromList(Node* list[], uint32_t* size, Node* node);
void updateObstacle(uint32_t x, uint32_t y, bool walkable);
void updatePlayerPos(Object* p1, Object* p2);
void updateEnemyNodePos(Node* enemy, uint32_t x, uint32_t y);
bool aStar(Node* start, Node* goal, uint32_t* nextX, uint32_t* nextY);