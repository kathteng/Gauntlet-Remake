#include <stdio.h>
#include <assert.h>
#include "SOIL.h"

#include "UIManager.h"
#include "defs.h"



static char START_SCREEN[] = "asset/ui/start_screen.png";
static char CHAR_SELECT_SCREEN[] = "asset/ui/character_select_screen.png";
static char HUD_SPRITE[] = "asset/ui/hud_bg.png";
static char LVL_TRANS[] = "asset/ui/level_transition.png";
static char PLAYER_NAMES_SPRITE[] = "asset/ui/character_names_spritesheet.png";
static char NUMBERS_SPRITE[] = "asset/ui/numbers_spritesheet.png";

static void drawStart(UIManager* ui);
static void drawCharSelect(UIManager* ui);
static void drawHUD(UIManager* ui);
static void drawScore1(UIManager* ui);
static void drawScore2(UIManager* ui);
static void drawHealth1(UIManager* ui);
static void drawHealth2(UIManager* ui);
static void drawPotions1(UIManager* ui);
static void drawPotions2(UIManager* ui);
static void drawKeys1(UIManager* ui);
static void drawKeys2(UIManager* ui);
static void drawLvlTrans(UIManager* ui);

void uimngrInitTextures(UIManager* ui)
{
	if (ui->startScreenTextures == 0)
	{
		ui->startScreenTextures = SOIL_load_OGL_texture(START_SCREEN, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(ui->startScreenTextures != 0);
	}

	if (ui->charSelectTextures == 0)
	{
		ui->charSelectTextures = SOIL_load_OGL_texture(CHAR_SELECT_SCREEN, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(ui->charSelectTextures != 0);
	}

	if (ui->hudTextures == 0)
	{
		ui->hudTextures = SOIL_load_OGL_texture(HUD_SPRITE, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(ui->hudTextures != 0);
	}

	if (ui->lvlTransTextures == 0)
	{
		ui->lvlTransTextures = SOIL_load_OGL_texture(LVL_TRANS, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(ui->lvlTransTextures != 0);
	}

	if (ui->playerTextures == 0)
	{
		ui->playerTextures = SOIL_load_OGL_texture(PLAYER_NAMES_SPRITE, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(ui->playerTextures != 0);
	}

	if (ui->numbersTextures == 0)
	{
		ui->numbersTextures = SOIL_load_OGL_texture(NUMBERS_SPRITE, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(ui->numbersTextures != 0);
	}

	if (ui->itemsTextures == 0)
	{
		ui->itemsTextures = SOIL_load_OGL_texture(COLLECTIBLES_SPRITESHEET, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
			SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
		assert(ui->itemsTextures != 0);
	}
}

UIManager* uimngrNew()
{
	UIManager* ui = malloc(sizeof(UIManager));
	if (ui != NULL)
	{
		ui->isStartScreen = true;
		ui->isCharSelect = false;
		ui->isHUD = false;
		ui->levelTrans = false;
		ui->isGameOver = false;

		ui->offsetX = 0.0f;
		ui->offsetY = 0.0f;
		ui->levelTransTimer = 0.0f;

		ui->levelNum = 1;
		ui->p1 = 0;
		ui->p2 = 0;
		ui->p1Health = PLAYER_MAX_HEALTH;
		ui->p2Health = PLAYER_MAX_HEALTH;
		ui->p1Score = 0;
		ui->p2Score = 0;
		ui->p1PotionCount = 0;
		ui->p2PotionCount = 0;
		ui->p1KeyCount = 0;
		ui->p2KeyCount = 0;

		ui->startScreenTextures = 0;
		ui->charSelectTextures = 0;
		ui->playerTextures = 0;
		ui->numbersTextures = 0;
		ui->hudTextures = 0;
		ui->lvlTransTextures = 0;
		ui->itemsTextures = 0;
		uimngrInitTextures(ui);
	}

	return ui;
}

void uimngrDelete(UIManager* ui)
{
	free(ui);
}

void uimngrSetPlayers(UIManager* ui, uint8_t p1, uint8_t p2)
{
	ui->p1 = p1;
	ui->p2 = p2;
}

void uimngrDraw(UIManager* ui)
{
	if (ui->isStartScreen)
	{
		drawStart(ui);
	}

	if (ui->isCharSelect)
	{
		drawCharSelect(ui);
	}

	if (ui->isHUD)
	{
		drawHUD(ui);
		drawHealth1(ui);
		drawHealth2(ui);
		drawScore1(ui);
		drawScore2(ui);
		drawPotions1(ui);
		drawPotions2(ui);
		drawKeys1(ui);
		drawKeys2(ui);
	}

	if (ui->levelTrans)
	{
		drawLvlTrans(ui);
	}
}

static void drawStart(UIManager* ui)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->startScreenTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = 0.0f;
		GLfloat xPosRight = SCREEN_WIDTH;
		GLfloat yPosTop = 0.0f;
		GLfloat yPosBottom = SCREEN_HEIGHT;

		const float BG_DEPTH = 1.0f;

		//tex coords are bottom left to top right
		// draw the textured quad as a tristrip
		glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

		// TL
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(xPosLeft, yPosTop, BG_DEPTH);

		// BL
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(xPosLeft, yPosBottom, BG_DEPTH);

		// TR
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(xPosRight, yPosTop, BG_DEPTH);

		// BR
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(xPosRight, yPosBottom, BG_DEPTH);
	}
	glEnd();
}

static void drawCharSelect(UIManager* ui)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->charSelectTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = 0.0f;
		GLfloat xPosRight = SCREEN_HEIGHT;
		GLfloat yPosTop = 0.0f;
		GLfloat yPosBottom = SCREEN_HEIGHT;

		const float BG_DEPTH = 1.0f;

		//tex coords are bottom left to top right
		// draw the textured quad as a tristrip
		glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

		// TL
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(xPosLeft, yPosTop, BG_DEPTH);

		// BL
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(xPosLeft, yPosBottom, BG_DEPTH);

		// TR
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(xPosRight, yPosTop, BG_DEPTH);

		// BR
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(xPosRight, yPosBottom, BG_DEPTH);
	}
	glEnd();
}

static void drawHUD(UIManager* ui)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->hudTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = SCREEN_HEIGHT + ui->offsetX;
		GLfloat xPosRight = SCREEN_WIDTH + ui->offsetX;
		GLfloat yPosTop = 0.0f + ui->offsetY;
		GLfloat yPosBottom = SCREEN_HEIGHT + ui->offsetY;

		const float BG_DEPTH = 0.99f;

		//tex coords are bottom left to top right
		// draw the textured quad as a tristrip
		glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

		// TL
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(xPosLeft, yPosTop, BG_DEPTH);

		// BL
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(xPosLeft, yPosBottom, BG_DEPTH);

		// TR
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(xPosRight, yPosTop, BG_DEPTH);

		// BR
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(xPosRight, yPosBottom, BG_DEPTH);
	}
	glEnd();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->playerTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = SCREEN_HEIGHT + 30.0f + ui->offsetX;
		GLfloat xPosRight = SCREEN_WIDTH - 30.0f + ui->offsetX;
		GLfloat yPosTop = 70.0f + ui->offsetY;
		GLfloat yPosBottom = 100.0f + ui->offsetY;

		float uPerChar = 1.0f / 4.0f;

		GLfloat xTexCoord = ui->p1 * uPerChar;

		const float BG_DEPTH = 1.0f;

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

	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = SCREEN_HEIGHT + 30.0f + ui->offsetX;
		GLfloat xPosRight = SCREEN_WIDTH - 30.0f + ui->offsetX;
		GLfloat yPosTop = 420.0f + ui->offsetY;
		GLfloat yPosBottom = 450.0f + ui->offsetY;

		float uPerChar = 1.0f / 4.0f;

		GLfloat xTexCoord = ui->p2 * uPerChar;

		const float BG_DEPTH = 1.0f;

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

static void drawScore1(UIManager* ui)
{
	char score[6];
	sprintf_s(score, sizeof(score), "%d", ui->p1Score);
	uint32_t scoreLength = strlen(score);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->numbersTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// score 1 ones digit
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = HEALTH_SCORE_XL + ui->offsetX;
		GLfloat xPosRight = HEALTH_SCORE_XR + ui->offsetX;
		GLfloat yPosTop = SCORE1_YTOP + ui->offsetY;
		GLfloat yPosBottom = SCORE1_YTOP + 20.0f + ui->offsetY;

		float uPerChar = 1.0f / 10.0f;

		uint32_t ones = score[scoreLength - 1] - '0';
		
		GLfloat xTexCoord = ones * uPerChar;

		const float BG_DEPTH = 1.0f;

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

	if (scoreLength > 1)
	{
		// score 1 tens digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 20.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 20.0f + ui->offsetX;
			GLfloat yPosTop = SCORE1_YTOP + ui->offsetY;
			GLfloat yPosBottom = SCORE1_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t tens = score[strlen(score) - 2] - '0';

			GLfloat xTexCoord = tens * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (scoreLength > 2)
	{
		// score 1 hundreds digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 40.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 40.0f + ui->offsetX;
			GLfloat yPosTop = SCORE1_YTOP + ui->offsetY;
			GLfloat yPosBottom = SCORE1_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t hundreds = score[strlen(score) - 3] - '0';

			GLfloat xTexCoord = hundreds * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (scoreLength > 3)
	{
		// score 1 thousands digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 60.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 60.0f + ui->offsetX;
			GLfloat yPosTop = SCORE1_YTOP + ui->offsetY;
			GLfloat yPosBottom = SCORE1_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t thousands = score[strlen(score) - 4] - '0';

			GLfloat xTexCoord = thousands * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (scoreLength > 4)
	{
		// score 1 ten thousands digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 80.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 80.0f + ui->offsetX;
			GLfloat yPosTop = SCORE1_YTOP + ui->offsetY;
			GLfloat yPosBottom = SCORE1_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t thousands = score[strlen(score) - 5] - '0';

			GLfloat xTexCoord = thousands * uPerChar;

			const float BG_DEPTH = 1.0f;

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

static void drawScore2(UIManager* ui)
{
	char score[6];
	sprintf_s(score, sizeof(score), "%d", ui->p2Score);
	uint32_t scoreLength = strlen(score);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->numbersTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// score 1 ones digit
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = HEALTH_SCORE_XL + ui->offsetX;
		GLfloat xPosRight = HEALTH_SCORE_XR + ui->offsetX;
		GLfloat yPosTop = SCORE2_YTOP + ui->offsetY;
		GLfloat yPosBottom = SCORE2_YTOP + 20.0f + ui->offsetY;

		float uPerChar = 1.0f / 10.0f;

		uint32_t ones = score[scoreLength - 1] - '0';

		GLfloat xTexCoord = ones * uPerChar;

		const float BG_DEPTH = 1.0f;

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

	if (scoreLength > 1)
	{
		// score 1 tens digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 20.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 20.0f + ui->offsetX;
			GLfloat yPosTop = SCORE2_YTOP + ui->offsetY;
			GLfloat yPosBottom = SCORE2_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t tens = score[strlen(score) - 2] - '0';

			GLfloat xTexCoord = tens * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (scoreLength > 2)
	{
		// score 1 hundredths digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 40.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 40.0f + ui->offsetX;
			GLfloat yPosTop = SCORE2_YTOP + ui->offsetY;
			GLfloat yPosBottom = SCORE2_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t tens = score[strlen(score) - 3] - '0';

			GLfloat xTexCoord = tens * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (scoreLength > 3)
	{
		// score 1 thousandths digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 60.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 60.0f + ui->offsetX;
			GLfloat yPosTop = SCORE2_YTOP + ui->offsetY;
			GLfloat yPosBottom = SCORE2_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t tens = score[strlen(score) - 4] - '0';

			GLfloat xTexCoord = tens * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (scoreLength > 4)
	{
		// score 1 ten thousands digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 80.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 80.0f + ui->offsetX;
			GLfloat yPosTop = SCORE2_YTOP + ui->offsetY;
			GLfloat yPosBottom = SCORE2_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t thousands = score[strlen(score) - 5] - '0';

			GLfloat xTexCoord = thousands * uPerChar;

			const float BG_DEPTH = 1.0f;

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

static void drawHealth1(UIManager* ui)
{
	char health[5];
	sprintf_s(health, sizeof(health), "%d", ui->p1Health);
	uint32_t healthLength = strlen(health);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->numbersTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// ones digit
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = HEALTH_SCORE_XL + ui->offsetX;
		GLfloat xPosRight = HEALTH_SCORE_XR + ui->offsetX;
		GLfloat yPosTop = HEALTH1_YTOP + ui->offsetY;
		GLfloat yPosBottom = HEALTH1_YTOP + 20.0f + ui->offsetY;

		float uPerChar = 1.0f / 10.0f;

		uint32_t ones = health[healthLength - 1] - '0';

		GLfloat xTexCoord = ones * uPerChar;

		const float BG_DEPTH = 1.0f;

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

	if (healthLength > 1)
	{
		// tens digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 20.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 20.0f + ui->offsetX;
			GLfloat yPosTop = HEALTH1_YTOP + ui->offsetY;
			GLfloat yPosBottom = HEALTH1_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t tens = health[healthLength - 2] - '0';

			GLfloat xTexCoord = tens * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (healthLength > 2)
	{
		// hundreds digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 40.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 40.0f + ui->offsetX;
			GLfloat yPosTop = HEALTH1_YTOP + ui->offsetY;
			GLfloat yPosBottom = HEALTH1_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t hundreds = health[healthLength - 3] - '0';

			GLfloat xTexCoord = hundreds * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (healthLength > 3)
	{
		// thousands digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 60.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 60.0f + ui->offsetX;
			GLfloat yPosTop = HEALTH1_YTOP + ui->offsetY;
			GLfloat yPosBottom = HEALTH1_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t thousands = health[healthLength - 4] - '0';

			GLfloat xTexCoord = thousands * uPerChar;

			const float BG_DEPTH = 1.0f;

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

static void drawHealth2(UIManager* ui)
{
	char health[5];
	sprintf_s(health, sizeof(health), "%d", ui->p2Health);
	uint32_t healthLength = strlen(health);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->numbersTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// ones digit
	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = HEALTH_SCORE_XL + ui->offsetX;
		GLfloat xPosRight = HEALTH_SCORE_XR + ui->offsetX;
		GLfloat yPosTop = HEALTH2_YTOP + ui->offsetY;
		GLfloat yPosBottom = HEALTH2_YTOP + 20.0f + ui->offsetY;

		float uPerChar = 1.0f / 10.0f;

		uint32_t ones = health[healthLength - 1] - '0';

		GLfloat xTexCoord = ones * uPerChar;

		const float BG_DEPTH = 1.0f;

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

	if (healthLength > 1)
	{
		// tens digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 20.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 20.0f + ui->offsetX;
			GLfloat yPosTop = HEALTH2_YTOP + ui->offsetY;
			GLfloat yPosBottom = HEALTH2_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t tens = health[healthLength - 2] - '0';

			GLfloat xTexCoord = tens * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (healthLength > 2)
	{
		// hundreds digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 40.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 40.0f + ui->offsetX;
			GLfloat yPosTop = HEALTH2_YTOP + ui->offsetY;
			GLfloat yPosBottom = HEALTH2_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t hundreds = health[healthLength - 3] - '0';

			GLfloat xTexCoord = hundreds * uPerChar;

			const float BG_DEPTH = 1.0f;

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
	if (healthLength > 3)
	{
		// thousands digit
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = HEALTH_SCORE_XL - 60.0f + ui->offsetX;
			GLfloat xPosRight = HEALTH_SCORE_XR - 60.0f + ui->offsetX;
			GLfloat yPosTop = HEALTH2_YTOP + ui->offsetY;
			GLfloat yPosBottom = HEALTH2_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / 10.0f;

			uint32_t thousands = health[healthLength - 4] - '0';

			GLfloat xTexCoord = thousands * uPerChar;

			const float BG_DEPTH = 1.0f;

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

static void drawPotions1(UIManager* ui)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->itemsTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	for (uint32_t i = 0; i < ui->p1PotionCount; ++i)
	{
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = POTION_XL + 20.0f * i + ui->offsetX;
			GLfloat xPosRight = POTION_XR + 20.0f * i + ui->offsetX;
			GLfloat yPosTop = POTION1_YTOP + ui->offsetY;
			GLfloat yPosBottom = POTION1_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / COLLECTIBLES_COUNT;

			GLfloat xTexCoord = 3.0f * uPerChar;

			const float BG_DEPTH = 1.0f;

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

static void drawPotions2(UIManager* ui)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->itemsTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (uint32_t i = 0; i < ui->p2PotionCount; ++i)
	{
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = POTION_XL + 20.0f * i + ui->offsetX;
			GLfloat xPosRight = POTION_XR + 20.0f * i + ui->offsetX;
			GLfloat yPosTop = POTION2_YTOP + ui->offsetY;
			GLfloat yPosBottom = POTION2_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / COLLECTIBLES_COUNT;

			GLfloat xTexCoord = 3.0f * uPerChar;

			const float BG_DEPTH = 1.0f;

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

static void drawKeys1(UIManager* ui)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->itemsTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (uint32_t i = 0; i < ui->p1KeyCount; ++i)
	{
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = POTION_XL + 20.0f * i + ui->offsetX;
			GLfloat xPosRight = POTION_XR + 20.0f * i + ui->offsetX;
			GLfloat yPosTop = KEY1_YTOP + ui->offsetY;
			GLfloat yPosBottom = KEY1_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / COLLECTIBLES_COUNT;

			GLfloat xTexCoord = 0.0f * uPerChar;

			const float BG_DEPTH = 1.0f;

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

static void drawKeys2(UIManager* ui)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->itemsTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	for (uint32_t i = 0; i < ui->p2KeyCount; ++i)
	{
		glBegin(GL_TRIANGLE_STRIP);
		{
			GLfloat xPosLeft = POTION_XL + 20.0f * i + ui->offsetX;
			GLfloat xPosRight = POTION_XR + 20.0f * i + ui->offsetX;
			GLfloat yPosTop = KEY2_YTOP + ui->offsetY;
			GLfloat yPosBottom = KEY2_YTOP + 20.0f + ui->offsetY;

			float uPerChar = 1.0f / COLLECTIBLES_COUNT;

			GLfloat xTexCoord = 0.0f * uPerChar;

			const float BG_DEPTH = 1.0f;

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

static void drawLvlTrans(UIManager* ui)
{
	// background
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->lvlTransTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = 0.0f + ui->offsetX;
		GLfloat xPosRight = SCREEN_HEIGHT + ui->offsetX;
		GLfloat yPosTop = 0.0f + ui->offsetY;
		GLfloat yPosBottom = SCREEN_HEIGHT + ui->offsetY;

		const float BG_DEPTH = 0.99f;

		//tex coords are bottom left to top right
		// draw the textured quad as a tristrip
		glColor4ub(0xFF, 0xFF, 0xFF, 0xFF);

		// TL
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(xPosLeft, yPosTop, BG_DEPTH);

		// BL
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(xPosLeft, yPosBottom, BG_DEPTH);

		// TR
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(xPosRight, yPosTop, BG_DEPTH);

		// BR
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(xPosRight, yPosBottom, BG_DEPTH);
	}
	glEnd();

	// level number
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ui->numbersTextures);
	// disable anti-aliasing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBegin(GL_TRIANGLE_STRIP);
	{
		GLfloat xPosLeft = SCREEN_WIDTH - 450 + ui->offsetX;
		GLfloat xPosRight = SCREEN_WIDTH - 450 + 58 + ui->offsetX;
		GLfloat yPosTop = 340.0f + ui->offsetY;
		GLfloat yPosBottom = 349.0f + 71 + ui->offsetY;

		float uPerChar = 1.0f / 10.0f;

		GLfloat xTexCoord = ui->levelNum * uPerChar;

		const float BG_DEPTH = 1.0f;

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

void uimngrReset(UIManager* ui)
{
	ui->isStartScreen = true;
	ui->isCharSelect = false;
	ui->isHUD = false;
	ui->levelTrans = false;
	ui->isGameOver = false;

	ui->offsetX = 0.0f;
	ui->offsetY = 0.0f;

	ui->levelNum = 1;
	ui->p1 = 0;
	ui->p2 = 0;
	ui->p1Health = PLAYER_MAX_HEALTH;
	ui->p2Health = PLAYER_MAX_HEALTH;
	ui->p1Score = 0;
	ui->p2Score = 0;
	ui->p1PotionCount = 0;
	ui->p2PotionCount = 0;
	ui->p1KeyCount = 0;
	ui->p2KeyCount = 0;
}