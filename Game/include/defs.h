#pragma once
#include <math.h>


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define LEVEL_SIZE 33
#define LEVEL_TILES LEVEL_SIZE*LEVEL_SIZE
#define TILE_SIZE 64.0f
#define TILES_ON_SCREEN 8
#define PROJECT_SIZE 32.0f

#define PLAYER_MAX_HEALTH 999
#define PLAYER1_SPAWN_POSX 160.0f
#define PLAYER_SPAWN_POSY 160.0f
#define PLAYER2_SPAWN_POSX 224.0f
#define MAX_PLAYER_DIST SCREEN_HEIGHT - 500

#define P1_POS '1'
#define P2_POS '2'
#define WALL '#'
#define GROUND ' '
#define KEY 'K'
#define DOOR_V 'D'
#define DOOR_H 'd'
#define EXIT 'X'
#define POTION 'P'
#define TREASURE 'T'
#define FOOD 'F'
#define GHOST_SPAWN 'G'
#define GRUNT_SPAWN 'R'
#define DEATH_SPAWN 'H'

#define DIR_COUNT 8
#define SQRT2 sqrtf(2.0f)
#define PROJECT_SPEED 1.0f

#define MAX_EXITS 3
#define MAX_ITEMS 30

#define ANIM_FRAME_RATE 1000.0f / 10.0f
#define MAX_ENEMIES 100
#define MAX_DEATHS 4
#define ENEMY_SPAWN_MAX 4
#define MAX_ENEMY_SPAWNERS 30

#define COLLECTIBLES_SPRITESHEET "asset/collectibles_spritesheet.png"
#define COLLECTIBLES_COUNT 4
#define POTION_DAMAGE 10
#define POTION_RADIUS 1000.0f

#define STRAIGHT_COST 10.0f
#define DIAGONAL_COST 14.0f

#define HEALTH_SCORE_XL SCREEN_WIDTH - 110.0f
#define HEALTH_SCORE_XR SCREEN_WIDTH - 90.0f
#define HEALTH1_YTOP 150.0f
#define HEALTH2_YTOP HEALTH1_YTOP + 350
#define SCORE1_YTOP 230.0f
#define SCORE2_YTOP SCORE1_YTOP + 350
#define POTION_XL SCREEN_WIDTH - 240.0f
#define POTION_XR POTION_XL + 20.0f
#define POTION1_YTOP 300.0f
#define POTION2_YTOP POTION1_YTOP + 350
#define KEY1_YTOP 340.0f
#define KEY2_YTOP KEY1_YTOP + 350