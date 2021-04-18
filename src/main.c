#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>

#include "asset.h"

// START CATZAP
#include "sneswin.h"

#include "commontypes.h"
#include "definitions.h"
#include "sounds.h"

void checkApexPredator(u8 type);
void checkPoweredUp(u8 type);

void victory();

//shake
void shakeScreen(u8 strength);
void updateShake();

//helper
void updateScore();
void updateTimer();
void fade_to(u8 to, u8 fadeDelay);
void killPlayer();
void hitPlayer();
void killEnemyBullet(u8 index);
void killPlayerBullet(u8 index);
void killEnemy(u8 index, u8 killPlayer);
u8 collidesWithBullet(u8 dist);
u8 outsideBoundaries();
void drawScoreSprite();
void putsSprite(u8 x, u8 y, const u8 str[]);

//explosion
void createSmallExplosion(u8);
void initExplosion();
void updateExplosion();
void createAOE();
void updateAOE();
void createFlameExplosion();
void createLargeExplosion();
void createExplosion(u8 move, u8 type);
void createSmallExplosion(u8 move);
void createPlayerRandomExplosion();
// void createSteam();

//calculate_homming
void calculateHomming();

//player
void cleanupWeapons();
void resetWeapons();
void initPlayer();
void nextWeapon(u8 finish);
void previousWeapon();
void spawnPlayerBulletEx(u8 selectedWeapon);
void spawnPlayerBullet();
void compareDistance();
void reacquireTarget();
void spawnPlayerRailgun();
void checkWeapon(u16* weapon);
void updateWeapon();
void updatePlayer();
u8 getClosestEnemyTarget();
u8 getClosestEnemyBullet();

//game
void vram_write_bank(u8 bank, const u8* src, unsigned int size);
void loadCommon();
void loadLevel();
void showBase();
void showPlayer(u8 show);
void updatePlayerDead();

//enemy
void spawnChild();
void spawnCawCawAggressor();
void spawnCawCawBomber();
void spawnCawCawFighter();
void spawnAsteroid();
void spawnSmallAsteroid(u8 parent, u8 direction);
void spawnSpawner();
u8 spawnSpawnerChild();
void spawnCCShiny();
void spawnRattaLauncher();
void spawnRattaSwappers();
void spawnCCDestroyer();
void spawnZigZag();
void spawnStreamer();
void spawnTrickster();
void spawnSpaceSerpent();
void spawnSwarmCloak();
void spawnAlphaCaw();
void spawnRattungandr();
void updateCawCawBomber();
void updateCawCawFighter();

void enemyReset(u8 index);
void initEnemies();
void enemyShoot();
void enemyShootEx(u8 direction);
void spawnEnemyEx(u8 type, u8 direction, u8 parent, u8 location);
void spawnEnemy(u8 type);
void updateSpawner();
void spawnLevelEnemy();
void updateSpawns();
void updateEnemies();
u8 enemyCollidesWithPlayer(u8 radius);

void shootSerpent(s8 offsetX, s8 offsetY, u8 bulletDirectionIndex);

//bullet_player
void initPlayerBullet();
void updateBulletTarget();
void updateHommingMissile();
void updatePlayerBullets();
u8 playerBulletCollidesWithEnemy();
u8 playerBulletCollidesWithEnemyBullet();
u8 checkWallCollision();
u8 playerBulletCollidesWithWall();
u8 playerBulletCollidesWithItem();

//bullet_enemy
void initEnemyBullet();
void spawnEnemyBulletType(u8 type, u8 direction);
void spawnEnemyBullet(u8 direction);
void spawnEnemyBulletHomming();
void spawnEnemyBulletCircling();
void spawnEnemyBulletEdging();
void updateEnemyBullets();
u8 enemyBulletCollidesWithPlayer();

//wall
void initWall();
void spawnWall(u8 direction);
void updateWall();
void updateWallIndex();

//item
void initItem();
void getItem(u8 index);
void spawnItem(u8 index);
void updateItem();

//score
void drawScore();
void drawScoreBG();
void drawTimer();
void putsBG(const u8 str[]);
void cleanupBGText(u8 y);
void cleanupBG3();
void drawTimerSprite();
void drawScoreSpr();
void checkPause();

//movement
void updatePop();
void shootMiddle();
void updateEnemyEdgeMovement(u8 shootEdge);

void initCircling();
void updateCircling(u8 dives, u8 shoots, u8 turns, u8 angleSpeed);
void checkDive(u8 turns);

void updateBulletdgeMovement();
void dive();
void updateDiving();
#pragma wrapped-call(pop)

#pragma wrapped-call(push, trampoline, 0x84)
u8* controlToASCII(u16 control);
void putsBGEx(u8 x, u8 y, const u8 str[], u8 pal, u8 buf);
void options_screen();
#pragma wrapped-call(pop)

//mode7
void setMode7Rot(u8);
void init_mode7();
void update_mode7();

//stars
void initStars();
void drawStars();
void updateArmada();

void render_level();
void drawLivesSpr();

#include "highscoreStruct.h"
#include "sprites.h"
#include "data.h"
#include "globalzp.h"
#include "globalbss.h"
#include "text.h"
#include "mode7.h"
#include "shake.h"
#include "helper.h"
#include "explosion.h"
#include "common_homming.h"
#include "player.h"
#include "enemy.h"
#include "wall.h"
#include "bullet_player.h"
#include "bullet_enemy.h"
#include "score.h"
#include "item.h"
#include "scroll.h"
#include "load.h"
#include "achievements_helper.h"
#include "achievements.h"
#include "highscore.h"
#include "options.h"
#include "stars.h"
//#include "title.h"
//#include "gameover.h"
#include "game.h"

#define SCREEN_WIDTH  512
#define SCREEN_HEIGHT  448

// END CATZAP

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
bool running = true;

void render();

Asset sprites[256];
Asset numbersprite;

extern bool game_done;
extern bool game_clear;



bool initSDL() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		return false;
	}

	window = SDL_CreateWindow(
		"CatZap",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_SHOWN
	);
	if (!window) {
		return false;
	}

	renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (!renderer) {
		return false;
	}

	SDL_RenderSetScale(renderer,
		2,
		2);

	return true;
}

void shutdown() {
	if (renderer) {
		SDL_DestroyRenderer(renderer);
	}
	if (window) {
		SDL_DestroyWindow(window);
	}

	SDL_Quit();
}

void render() {
	SDL_RenderClear(renderer);
	/*updateTimer();
	updateLevel();
	renderMap();*/
	update_game();

	SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
	if (!initSDL()) {
		printf("Window could not be created!\n"
			"SDL_Error: %s\n", SDL_GetError());
	}

	keys[KEY_SHOOT] = PAD_A;
	keys[KEY_SPECIAL] = PAD_X;
	keys[KEY_PREVIOUS_WEAPON] = PAD_L;
	keys[KEY_NEXT_WEAPON] = PAD_R;

	loadAssets();
	lives = 3;
	game_level = BOSS_2;

	auto last_time = SDL_GetTicks();
	while (running) {
		init_game();
		while (!game_done) {
			auto time_now = SDL_GetTicks();
			auto elapsed = time_now - last_time;
			last_time = time_now;

			SDL_Event event;
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					running = false;
				}
			}
			if (!running) {
				break;
			}

			render();
			if (game_clear)
			{
				++game_level;
			}
		}
	}

	shutdown();

	return EXIT_SUCCESS;
}



