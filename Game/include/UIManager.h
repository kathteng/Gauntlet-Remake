#pragma once
#include <Windows.h>
#include <gl/GLU.h>

#include "baseTypes.h"


typedef struct uimanager_t UIManager;

typedef struct uimanager_t
{
	bool isStartScreen;
	bool isCharSelect;
	bool isHUD;
	bool isGameOver;
	bool levelTrans;

	float offsetX;
	float offsetY;
	float levelTransTimer;

	uint32_t levelNum;
	uint8_t p1;
	uint8_t p2;
	int32_t p1Health;
	int32_t p2Health;
	uint32_t p1Score;
	uint32_t p2Score;
	uint32_t p1PotionCount;
	uint32_t p2PotionCount;
	uint32_t p1KeyCount;
	uint32_t p2KeyCount;

	GLuint startScreenTextures;
	GLuint charSelectTextures;
	GLuint playerTextures;
	GLuint numbersTextures;
	GLuint hudTextures;
	GLuint lvlTransTextures;
	GLuint itemsTextures;
} UIManager;

void uimngrInitTextures(UIManager* ui);
UIManager* uimngrNew();
void uimngrDelete(UIManager* ui);
void uimngrSetPlayers(UIManager* ui, uint8_t p1, uint8_t p2);
void uimngrDraw(UIManager* ui);
void uimngrReset(UIManager* ui);