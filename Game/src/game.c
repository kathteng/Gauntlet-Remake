#include <stdio.h>
#include "baseTypes.h"
#include "input.h"
#include "application.h"
#include "framework.h"

#include "defs.h"
#include "UIManager.h"
#include "objmgr.h"
#include "Level.h"
#include "Player.h"
#include "Tile.h"
#include "Astar.h"
#include "sound.h"
#include "DeathSpawner.h"

static void _gameInit();
static void _gameShutdown();
static void _gameDraw();
static void _gameUpdate(uint32_t milliseconds);
static void updateCamera();

static bool quit = false;
// for input latching on using potions
static bool potionUsed1 = false;
static bool potionUsed2 = false;

static int32_t introMusic = SOUND_NOSOUND;
static int32_t welcomeSound = SOUND_NOSOUND;
static int32_t dieSound = SOUND_NOSOUND;
static int32_t doorOpenSound = SOUND_NOSOUND;
static int32_t keyPickupSound = SOUND_NOSOUND;
static int32_t treasureSound = SOUND_NOSOUND;
static int32_t potionUseSound = SOUND_NOSOUND;
static int32_t exitSound = SOUND_NOSOUND;

static UIManager* _ui = NULL;
static Level* _curLevel = NULL;
static Player* _player1 = NULL;
static Player* _player2 = NULL;

static uint8_t p1Char = 0;
static uint8_t p2Char = 0;

static float timer = 0.0f;
static float gameStartBuffer = 0.0f;
static float musicTimer = 0.0f;
static float gameOverTimer = 0.0f;

/// @brief Program Entry Point (WinMain)
/// @param hInstance 
/// @param hPrevInstance 
/// @param lpCmdLine 
/// @param nCmdShow 
/// @return 
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	const char GAME_NAME[] = "Gauntlet";

	Application* app = appNew(hInstance, GAME_NAME, _gameDraw, _gameUpdate);

	if (app != NULL)
	{
		GLWindow* window = fwInitWindow(app);
		if (window != NULL)
		{
			_gameInit();

			bool running = true;
			while (running)
			{
				running = fwUpdateWindow(window);
				if (quit)
					break;
			}

			_gameShutdown();
			fwShutdownWindow(window);
		}

		appDelete(app);
	}
}

/// @brief Initialize code to run at application startup
static void _gameInit()
{
	const uint32_t MAX_OBJECTS = 5000;
	objMgrInit(MAX_OBJECTS);

	_ui = uimngrNew();

	introMusic = soundLoad("asset/sounds/intro_music.wav");
	welcomeSound = soundLoad("asset/sounds/welcome.wav");
	dieSound = soundLoad("asset/sounds/die.wav");
	doorOpenSound = soundLoad("asset/sounds/dooropen.wav");
	keyPickupSound = soundLoad("asset/sounds/keypickup.wav");
	treasureSound = soundLoad("asset/sounds/treasure.wav");
	potionUseSound = soundLoad("asset/sounds/potionuse.wav");
	exitSound = soundLoad("asset/sounds/exit.wav");
	loadDeathSound();

	soundPlay(introMusic);
}

/// @brief Cleanup the game and free up any allocated resources
static void _gameShutdown()
{
	levelDelete(_curLevel);
	_curLevel = NULL;

	if (_player1 != NULL)
	{
		playerDelete(_player1);
		_player1 = NULL;
	}
	if (_player2 != NULL)
	{
		playerDelete(_player2);
		_player2 = NULL;
	}

	uimngrDelete(_ui);
	_ui = NULL;

	soundUnload(introMusic);
	soundUnload(welcomeSound);
	soundUnload(dieSound);
	soundUnload(doorOpenSound);
	soundUnload(keyPickupSound);
	soundUnload(treasureSound);
	soundUnload(potionUseSound);
	soundUnload(exitSound);
	unloadDeathSound();

	objMgrShutdown();
}

/// @brief Draw everything to the screen for current frame
static void _gameDraw() 
{
	updateCamera();

	if (!_ui->isGameOver && _curLevel != NULL)
		levelDraw(_curLevel);
	
	uimngrDraw(_ui);
	objMgrDraw();
}

/// @brief Perform updates for all game objects, for the elapsed duration
/// @param milliseconds 
static void _gameUpdate(uint32_t milliseconds)
{
	// replay intro music
	if (_ui->isStartScreen || _ui->isCharSelect)
		musicTimer += (milliseconds / 1000.0f);
	if (musicTimer > 70.0f && (_ui->isStartScreen || _ui->isCharSelect))
	{
		soundPlay(introMusic);
		musicTimer = 0.0f;
	}

	/* restart game ----------------------------------------*/
	if (_ui->isGameOver)
	{
		gameOverTimer += (milliseconds / 1000.0f);
		if (gameOverTimer >= 3.0f)
		{
			levelDelete(_curLevel);
			_curLevel = NULL;

			uimngrReset(_ui);
			gameOverTimer = 0.0f;
			soundPlay(introMusic);
		}
	}
	/*--------------------------------------------------*/

	// ESC exits the program
	if (inputKeyPressed(VK_ESCAPE))
	{
		quit = true;
	}

	/* start screen -----------------------------------------*/
	if (_ui->isStartScreen)
	{
		if (inputKeyPressed(VK_SPACE))
		{
			_ui->isStartScreen = false;
			_ui->isCharSelect = true;
			_ui->isHUD = true;
		}
	}
	/*------------------------------------------------------------------*/

	/* character select screen -----------------------------------------*/
	if (_ui->isCharSelect)
	{
		/* player 1 -----------------------------------------*/
		if (_player1 == NULL)
		{
			if (inputKeyPressed(VK_UP))
			{
				p1Char = WARRIOR;
			}
			else if (inputKeyPressed(VK_DOWN))
			{
				p1Char = ELF;
			}
			else if (inputKeyPressed(VK_LEFT))
			{
				p1Char = VALKYRIE;
			}
			else if (inputKeyPressed(VK_RIGHT))
			{
				p1Char = WIZARD;
			}
		}

		if (inputKeyPressed(0x58)) // X
		{
			if (_player1 == NULL && p1Char != p2Char)
			{
				Coord2D pos = { PLAYER1_SPAWN_POSX, PLAYER_SPAWN_POSY };
				_player1 = playerNew(p1Char, pos);
				soundPlay(welcomeSound);
			}
		}
		/*------------------------------------------------------------------*/

		/* player 2 -----------------------------------------*/
		if (_player2 == NULL)
		{
			if (inputKeyPressed(0x57)) // W up
			{
				p2Char = WARRIOR;
			}
			else if (inputKeyPressed(0x53)) // S down
			{
				p2Char = ELF;
			}
			else if (inputKeyPressed(0x41)) // A left
			{
				p2Char = VALKYRIE;
			}
			else if (inputKeyPressed(0x44)) // D right
			{
				p2Char = WIZARD;
			}
		}

		if (inputKeyPressed(0x49)) // I
		{
			if (_player2 == NULL && p1Char != p2Char)
			{
				Coord2D pos2 = { PLAYER2_SPAWN_POSX, PLAYER_SPAWN_POSY };
				_player2 = playerNew(p2Char, pos2);
				soundPlay(welcomeSound);
			}
		}
		/*------------------------------------------------------------------*/

		_ui->p1 = p1Char;
		_ui->p2 = p2Char;

		if (_player1 != NULL && _player2 != NULL)
		{
			gameStartBuffer += (milliseconds / 1000.0f);
		}

		// start game
		if (_player1 != NULL && _player2 != NULL && gameStartBuffer >= 1.0f)
		{
			_curLevel = levelNew(1);
			levelSetPlayerSpawns(_curLevel, (Object*)_player1, (Object*)_player2);
			nodeInitializePlayers((Object*)_player1, (Object*)_player2);
			_ui->isCharSelect = false;
			_ui->levelTrans = true;
			soundStop(introMusic);
		}
	}
	/*---------------------------------------------------------------------------------------*/

	if (_ui->levelTrans)
	{
		_ui->levelTransTimer += (milliseconds / 1000.0f);
	}

	if (_ui->levelTransTimer >= 2.0f)
	{
		levelDelete(_curLevel);
		_curLevel = levelNew(_ui->levelNum);
		levelSetPlayerSpawns(_curLevel, (Object*)_player1, (Object*)_player2);
		if (_player1 != NULL)
		{
			playerSetDirection(_player1, 0);
			projectileDeactivate(playerGetProjectile(_player1));
		}
		if (_player2 != NULL)
		{
			playerSetDirection(_player2, 0);
			projectileDeactivate(playerGetProjectile(_player2));
		}

		_ui->levelTrans = false;
		_ui->levelTransTimer = 0.0f;
	}

	/* game loop ----------------------------------------------------------------------------*/
	if (!_ui->isGameOver && !_ui->isStartScreen && !_ui->isCharSelect && !_ui->levelTrans)
	{
		/* decrease health per second -----------------------------*/
		timer += (milliseconds / 1000.0f);
		if (timer >= 1.0f)
		{
			if (_player1 != NULL)
			{
				// valkyrie takes half damage, so double it
				if (playerGetCharacter(_player1) == VALKYRIE)
					playerTakeDamage(_player1, 2);
				else
					playerTakeDamage(_player1, 1);
			}
			if (_player2 != NULL)
			{
				if (playerGetCharacter(_player2) == VALKYRIE)
					playerTakeDamage(_player2, 2);
				else
					playerTakeDamage(_player2, 1);
			}
			timer = 0.0f;
		}
		/*--------------------------------------------------------*/

		// check player deaths
		if (_player1 != NULL && playerGetHealth(_player1) <= 0)
		{
			soundPlay(dieSound);
			_ui->p1Health = 0;
			playerDelete(_player1);
			_player1 = NULL;
		}
		if (_player2 != NULL && playerGetHealth(_player2) <= 0)
		{
			soundPlay(dieSound);
			_ui->p2Health = 0;
			playerDelete(_player2);
			_player2 = NULL;
		}

		/* update UI ---------------------------------------------*/
		if (_player1 != NULL)
		{
			_ui->p1Health = playerGetHealth(_player1);
			_ui->p1Score = playerGetScore(_player1);
			_ui->p1KeyCount = playerGetKeyCount(_player1);
			_ui->p1PotionCount = playerGetPotionCount(_player1);
		}
		if (_player2 != NULL)
		{
			_ui->p2Health = playerGetHealth(_player2);
			_ui->p2Score = playerGetScore(_player2);
			_ui->p2KeyCount = playerGetKeyCount(_player2);
			_ui->p2PotionCount = playerGetPotionCount(_player2);
		}

		if (_player1 == NULL && _player2 == NULL)
		{
			_ui->isGameOver = true;

			levelDelete(_curLevel);
			_curLevel = NULL;
			return;
		}
		/*--------------------------------------------------------*/

		/* player 1 movement ---------------------------------------------------------------*/ 
		if (_player1 != NULL)
		{
			if (inputKeyPressed(VK_UP) && inputKeyPressed(VK_LEFT))
			{
				playerSetState(_player1, WALK);
				playerSetDirection(_player1, 5);
				playerSetVelocity(_player1, -1 / SQRT2, -1 / SQRT2);
			}
			else if (inputKeyPressed(VK_UP) && inputKeyPressed(VK_RIGHT))
			{
				playerSetState(_player1, WALK);
				playerSetDirection(_player1, 6);
				playerSetVelocity(_player1, 1 / SQRT2, -1 / SQRT2);
			}
			else if (inputKeyPressed(VK_DOWN) && inputKeyPressed(VK_LEFT))
			{
				playerSetState(_player1, WALK);
				playerSetDirection(_player1, 4);
				playerSetVelocity(_player1, -1 / SQRT2, 1 / SQRT2);
			}
			else if (inputKeyPressed(VK_DOWN) && inputKeyPressed(VK_RIGHT))
			{
				playerSetState(_player1, WALK);
				playerSetDirection(_player1, 7);
				playerSetVelocity(_player1, 1/ SQRT2, 1 / SQRT2);
			}
			else if (inputKeyPressed(VK_UP))
			{
				playerSetState(_player1, WALK);
				playerSetDirection(_player1, 2);
				playerSetVelocity(_player1, 0, -1);
			}
			else if (inputKeyPressed(VK_DOWN))
			{
				playerSetState(_player1, WALK);
				playerSetDirection(_player1, 0);
				playerSetVelocity(_player1, 0, 1);
			}
			else if (inputKeyPressed(VK_LEFT))
			{
				playerSetState(_player1, WALK);
				playerSetDirection(_player1, 1);
				playerSetVelocity(_player1, -1, 0);
			}
			else if (inputKeyPressed(VK_RIGHT))
			{
				playerSetState(_player1, WALK);
				playerSetDirection(_player1, 3);
				playerSetVelocity(_player1, 1, 0);
			}
			else
			{
				playerSetState(_player1, IDLE);
				playerSetVelocity(_player1, 0, 0);
			}

			// fire projectile
			if (inputKeyPressed(0x58)) // X
			{
				if (playerGetState(_player1) != WALK)
					playerSetState(_player1, ATTACK);
				playerFireProjectile(_player1);
			}

			// use potion
			if (inputKeyPressed(0x5A) && !potionUsed1) // Z
			{
				if (playerGetPotionCount(_player1) > 0)
				{
					levelUsePotion(_curLevel, _player1);
					playerUsePotion(_player1);
					potionUsed1 = true;
					soundPlay(potionUseSound);
				}
			}
			potionUsed1 = inputKeyPressed(0x4F);
			/* player 1 input end -----------------------------------------*/

			/* check collisions -----------------------------------------------------------------------------------------*/
			if (checkWallsCollision((Object*)_player1) || checkSpawnersCollisionPlayer(_curLevel, (Object*)_player1))
			{
				playerSetVelocity(_player1, 0, 0);
				playerResetPos(_player1);
			}

			if (checkDoorsCollision((Object*)_player1))
			{
				if (playerGetKeyCount(_player1) > 0)
				{
					levelOpenDoors(playerGetPos(_player1));
					playerUseKey(_player1);
					soundPlay(doorOpenSound);
					if (_player2 != NULL)
						playerUseKey(_player2);
				}
				else
				{
					playerSetVelocity(_player1, 0, 0);
					playerResetPos(_player1);
				}
			}
			/*------------------------------------------------------------*/
		}
		/*----------------------------------------------------------------------------------*/

		/* player 2 movement ---------------------------------------------------------------*/
		if (_player2 != NULL)
		{
			if (inputKeyPressed(0x57) && inputKeyPressed(0x41)) // W A
			{
				playerSetState(_player2, WALK);
				playerSetDirection(_player2, 5);
				playerSetVelocity(_player2, -1 / SQRT2, -1 / SQRT2);
			}
			else if (inputKeyPressed(0x57) && inputKeyPressed(0x44)) // W D
			{
				playerSetState(_player2, WALK);
				playerSetDirection(_player2, 6);
				playerSetVelocity(_player2, 1 / SQRT2, -1 / SQRT2);
			}
			else if (inputKeyPressed(0x53) && inputKeyPressed(0x41)) // S A
			{
				playerSetState(_player2, WALK);
				playerSetDirection(_player2, 4);
				playerSetVelocity(_player2, -1 / SQRT2, 1 / SQRT2);
			}
			else if (inputKeyPressed(0x53) && inputKeyPressed(0x44)) // S D
			{
				playerSetState(_player2, WALK);
				playerSetDirection(_player2, 7);
				playerSetVelocity(_player2, 1 / SQRT2, 1 / SQRT2);
			}
			else if (inputKeyPressed(0x57)) // W up
			{
				playerSetState(_player2, WALK);
				playerSetDirection(_player2, 2);
				playerSetVelocity(_player2, 0, -1);
			}
			else if (inputKeyPressed(0x53)) // S down
			{
				playerSetState(_player2, WALK);
				playerSetDirection(_player2, 0);
				playerSetVelocity(_player2, 0, 1);
			}
			else if (inputKeyPressed(0x41)) // A left
			{
				playerSetState(_player2, WALK);
				playerSetDirection(_player2, 1);
				playerSetVelocity(_player2, -1, 0);
			}
			else if (inputKeyPressed(0x44)) // D right
			{
				playerSetState(_player2, WALK);
				playerSetDirection(_player2, 3);
				playerSetVelocity(_player2, 1, 0);
			}
			else
			{
				playerSetState(_player2, IDLE);
				playerSetVelocity(_player2, 0, 0);
			}

			// fire projectile
			if (inputKeyPressed(0x49)) // I
			{
				if (playerGetState(_player2) != WALK)
					playerSetState(_player2, ATTACK);
				playerFireProjectile(_player2);
			}

			// use potion
			if (inputKeyPressed(0x4F) && !potionUsed2) // O
			{
				if (playerGetPotionCount(_player2) > 0)
				{
					levelUsePotion(_curLevel, _player2);
					playerUsePotion(_player2);
					soundPlay(potionUseSound);
					potionUsed2 = true;
				}
			}
			potionUsed2 = inputKeyPressed(0x45);
			/* player 2 input end -----------------------------------------*/

			/* check collisions -----------------------------------------------------------------------------------------*/
			if (checkWallsCollision((Object*)_player2) || checkSpawnersCollisionPlayer(_curLevel, (Object*)_player2))
			{
				playerSetVelocity(_player2, 0, 0);
				playerResetPos(_player2);
			}

			if (checkDoorsCollision((Object*)_player2))
			{
				if (playerGetKeyCount(_player2) > 0)
				{
					levelOpenDoors(playerGetPos(_player2));
					if (_player1 != NULL)
						playerUseKey(_player1);
					playerUseKey(_player2);
					soundPlay(doorOpenSound);
				}
				else
				{
					playerSetVelocity(_player2, 0, 0);
					playerResetPos(_player2);
				}
			}
			/*------------------------------------------------------------*/

			/*Coord2D pos = playerGetPos(_player1);
			Coord2D pos2 = playerGetPos(_player2);

			float dist = (float)fabs((pos2.y - pos.y) / (pos2.x - pos.x));
			if (dist > MAX_PLAYER_DIST)
			{
				playerSetVelocity(_player2, 0, 0);
				playerResetPos(_player2);
			}*/
		}
		/*----------------------------------------------------------------------------------*/

		/* check items collision ---------------------------------*/
		if (_player1 != NULL)
		{
			if (checkKeyCollect(_curLevel, _player1, _player2))
				soundPlay(keyPickupSound);
			if (checkTreasureCollect(_curLevel, _player1))
				soundPlay(treasureSound);
			if (checkFoodCollect(_curLevel, _player1))
				soundPlay(keyPickupSound);
			if (checkPotionCollect(_curLevel, _player1))
				soundPlay(keyPickupSound);
		}

		if (_player2 != NULL)
		{
			if (checkTreasureCollect(_curLevel, _player2))
				soundPlay(treasureSound);
			if (checkFoodCollect(_curLevel, _player2))
				soundPlay(keyPickupSound);
			if (checkPotionCollect(_curLevel, _player2))
				soundPlay(keyPickupSound);
		}
		/*-------------------------------------------------------*/

		/* check if player hit enemy ---------------------------------------------------------------------*/
		uint32_t potentialPoints = 0;
		if (_player1 != NULL)
		{
			checkEnemiesCollision(_curLevel, (Object*)_player1, (Object*)playerGetProjectile(_player1));
	
			potentialPoints = checkSpawnersCollisionProjectile(_curLevel, playerGetProjectile(_player1));
			if (potentialPoints > 0)
				playerAddScore(_player1, potentialPoints);
		}

		if (_player2 != NULL)
		{
			checkEnemiesCollision(_curLevel, (Object*)_player2, (Object*)playerGetProjectile(_player2));

			potentialPoints = checkSpawnersCollisionProjectile(_curLevel, playerGetProjectile(_player2));
			if (potentialPoints > 0)
				playerAddScore(_player2, potentialPoints);
		}
		/*------------------------------------------------------------------------------------------------*/

		levelUpdate(_curLevel, milliseconds);

		objMgrUpdate(milliseconds);

		updatePlayerPos((Object*)_player1, (Object*)_player2);

		/* check exit collision -------------------------------------------------------------*/
		uint8_t newLevel = 0;

		if (_player1 != NULL)
		{
			newLevel = checkExitsCollision(_curLevel, (Object*)_player1);
		}
		if (_player2 != NULL && newLevel == 0)
		{
			newLevel = checkExitsCollision(_curLevel, (Object*)_player2);
		}
		// if a player entered an exit tile
		if (newLevel > 0)
		{
			soundPlay(exitSound);

			if (newLevel == 10)
				newLevel = 1;

			_ui->levelTrans = true;
			_ui->levelNum = newLevel;
		}
		/*--------------------------------------------------------------------------------*/
	}
}

/// @brief translate the camera so that it follows players
static void updateCamera()
{
	Coord2D pos = { 0.0f, 0.0f };
	Coord2D pos2 = { 0.0f, 0.0f };
	float centerX = 0;
	float centerY = 0;
	if (_player1 != NULL)
	{
		pos = playerGetPos(_player1);
	}
	if (_player2 != NULL)
	{
		pos2 = playerGetPos(_player2);
	}
	if (_player1 != NULL && _player2 != NULL)
	{
		centerX = (pos.x + pos2.x) / 2.0f;
		centerY = (pos.y + pos2.y) / 2.0f;
	}
	else if (_player1 != NULL)
	{
		centerX = pos.x;
		centerY = pos.y;
	}
	else if (_player2 != NULL)
	{
		centerX = pos2.x;
		centerY = pos2.y;
	}

	float camX = fmaxf(SCREEN_WIDTH / 2.0f, fminf(centerX, LEVEL_SIZE * TILE_SIZE + 256 - SCREEN_WIDTH / 2.0f));
	float camY = fmaxf(SCREEN_HEIGHT / 2.0f, fminf(centerY, LEVEL_SIZE * TILE_SIZE - SCREEN_HEIGHT / 2.0f));

	_ui->offsetX = camX - SCREEN_WIDTH / 2.0f;
	_ui->offsetY = camY - SCREEN_HEIGHT / 2.0f;

	glTranslatef(-camX + SCREEN_WIDTH / 2.0f, -camY + SCREEN_HEIGHT / 2.0f, 0.0f);
}