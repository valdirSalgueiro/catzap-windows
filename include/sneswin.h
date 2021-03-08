#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "spritedef.h"
#include "commontypes.h"
#include "definitions.h"

#define TRUE 1
#define FALSE 0
#define NULL 0

#define PAD_R			0x10
#define PAD_L			0x20
#define PAD_X			0x40
#define PAD_A			0x80
#define PAD_RIGHT		0x100
#define PAD_LEFT		0x200
#define PAD_DOWN		0x400
#define PAD_UP			0x800
#define PAD_START		0x1000
#define PAD_SELECT		0x2000
#define PAD_Y			0x4000
#define PAD_B			0x8000

#define TILE_ATTR_FULL(pal, prio, flipv, fliph, tile) (tile | ((pal) << 10) | ((prio) << 13) | ((fliph) << 14) | ((flipv) << 15))

extern SDL_Renderer* renderer;
void renderSprite(int srcX, int srcY, int destX, int destY, int type);

void pal_all(const char* data) {}

// set bg palette only, data is 16 bytes array
void pal_bg(const char* data) {}

// set spr palette only, data is 16 bytes array
void pal_spr(const char* data) {}

// set a palette entry, index is 0..31
void pal_col(unsigned char index, unsigned char color) {}

void pal_bright(unsigned char bright) {}

void sfx_play(unsigned char sound) {}
void scroll(unsigned int x, unsigned int y) {}
void music_play(unsigned char bank, const unsigned char* data) {}

unsigned char rand8(void) {
	return rand() & 255;
}

void vram_adr(unsigned int adr) {}
void vram_write_bank(u8 bank, const u8* src, unsigned int size) {}
void vram_put(unsigned int n) {}
void vram_fill(unsigned int n, unsigned int len) {}
void memcpy_fast(void* dest, const void* src, unsigned int n) {}
void dma_queue(unsigned char bank, const unsigned char* src,
	unsigned int dst, unsigned int size) {}
void ppu_off(void) {}
void ppu_on_all(void) {}
void ppu_wait_nmi() {}

unsigned short snesmul(const unsigned char a, const unsigned char b) {
	return a * b;
}
unsigned short snesdiv(const unsigned short a, const unsigned char b) {
	return a / b;
}
unsigned short snesmod(const unsigned short a, const unsigned char b) {
	return a % b;
}

u8 normalizeType(u8 type)
{
	if (type >= SPRITE_ENEMY_INDEX + ENEMY_CAW_CAW_BOMBER && type <= SPRITE_ENEMY_INDEX + ENEMY_CAW_CAW_FIGHTER)
	{
		return SPRITE_LEVEL1_ENEMIES;
	}
	else if (type == SPRITE_SATELLITE || type == SPRITE_BULLET_ENEMY_INDEX 
		|| type == SPRITE_PLAYER_BULLET || type == (SPRITE_EXPLOSION_INDEX + 1)
		|| type == SPRITE_PLAYER_BULLET_RAILGUN
		|| type == SPRITE_PLAYER_BULLET_HOMMING)
	{
		return SPRITE_SPRITES;
	}
	return type;
}

void oam_meta_spr_x(int x, unsigned char y,
	const unsigned char* data, u8 type) {
	renderSprite(data[0], data[1], x, y, normalizeType(type));
}

void oam_meta_spr_x_high(int x, unsigned char y,
	const unsigned char* data, u8 type) {
	renderSprite(data[0], data[1], x, y, normalizeType(type));
}

#define radiansToDegrees(angleRadians) (angleRadians * 180.0 / M_PI)
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

int _sin(unsigned x) 
{
	//printf("%d,%f, %f\n", x, degreesToRadians(x), sin(degreesToRadians(x)));
	return sin(degreesToRadians(x)) * 256;
}
int _cos(unsigned x) 
{
	return cos(degreesToRadians(x)) * 256;
}

void delay(unsigned char frames) {}

#include "asset.h"
extern Asset sprites[];
extern Asset bg;
extern Asset hudsprite;
extern Asset numbersprite;
extern const u8* mapsptr;

void renderSprite(int srcX, int srcY, int destX, int destY, int type)
{
	if (!sprites[type].texture)
	{
		return;
	}

	SDL_Rect srcrect;
	srcrect.x = srcX * sprites[type].w;
	srcrect.y = srcY * sprites[type].h;
	srcrect.w = sprites[type].w;
	srcrect.h = sprites[type].h;

	SDL_Rect dstrect;
	dstrect.x = destX;
	dstrect.y = destY + 1;
	dstrect.w = sprites[type].w;
	dstrect.h = sprites[type].h;

	SDL_RenderCopy(renderer, sprites[type].texture, &srcrect, &dstrect);
}

void renderMap() {}

void oam_clear() {}

const Uint8* state;
Uint8 oldstate[SDL_NUM_SCANCODES];
int pad_trigger(unsigned char pad)
{
	state = SDL_GetKeyboardState(NULL);
	int triggerState = 0;
	if (state[SDL_SCANCODE_RETURN] && !oldstate[SDL_SCANCODE_RETURN])
		triggerState |= PAD_START;

	if (state[SDL_SCANCODE_UP] && !oldstate[SDL_SCANCODE_UP])
		triggerState |= PAD_UP;
	if (state[SDL_SCANCODE_DOWN] && !oldstate[SDL_SCANCODE_DOWN])
		triggerState |= PAD_DOWN;
	if (state[SDL_SCANCODE_LEFT] && !oldstate[SDL_SCANCODE_LEFT])
		triggerState |= PAD_LEFT;
	if (state[SDL_SCANCODE_RIGHT] && !oldstate[SDL_SCANCODE_RIGHT])
		triggerState |= PAD_RIGHT;

	if (state[SDL_SCANCODE_Z] && !oldstate[SDL_SCANCODE_Z])
		triggerState |= PAD_A;
	if (state[SDL_SCANCODE_X] && !oldstate[SDL_SCANCODE_X])
		triggerState |= PAD_B;

	if (state[SDL_SCANCODE_A] && !oldstate[SDL_SCANCODE_A])
		triggerState |= PAD_X;
	if (state[SDL_SCANCODE_S] && !oldstate[SDL_SCANCODE_S])
		triggerState |= PAD_Y;

	if (state[SDL_SCANCODE_Q] && !oldstate[SDL_SCANCODE_Q])
		triggerState |= PAD_L;
	if (state[SDL_SCANCODE_W] && !oldstate[SDL_SCANCODE_W])
		triggerState |= PAD_R;

	memcpy(oldstate, state, sizeof(Uint8) * SDL_NUM_SCANCODES);

	return triggerState;
}
int pad_state(unsigned char pad)
{
	state = SDL_GetKeyboardState(NULL);
	int padState = 0;
	if (state[SDL_SCANCODE_RETURN])
		padState |= PAD_START;

	if (state[SDL_SCANCODE_UP])
		padState |= PAD_UP;
	if (state[SDL_SCANCODE_DOWN])
		padState |= PAD_DOWN;
	if (state[SDL_SCANCODE_LEFT])
		padState |= PAD_LEFT;
	if (state[SDL_SCANCODE_RIGHT])
		padState |= PAD_RIGHT;
	return padState;
}

void loadSprite(const char* path, int w, int h, int type) {
	sprites[type].surface = IMG_Load(path);
	SDL_SetColorKey(sprites[type].surface, SDL_TRUE, SDL_MapRGB(sprites[type].surface->format, 0, 0, 0));
	sprites[type].texture = SDL_CreateTextureFromSurface(renderer, sprites[type].surface);
	sprites[type].w = w;
	sprites[type].h = h;
}

void loadAssets() {
	loadSprite("../../../cat-zap/res/sprites.png", 16, 16, SPRITE_SPRITES);
	loadSprite("../../../cat-zap/res/alphabet.png", 8, 16, SPRITE_ALPHABET);
	loadSprite("../../../cat-zap/res/base0.png", 48, 48, SPRITE_BASE);
	loadSprite("../../../cat-zap/res/gamefg.png", 64, 48, SPRITE_GAME_FG);
	loadSprite("../../../cat-zap/res/weaponicons.png", 16, 16, SPRITE_WEAPON_ICONS);
	loadSprite("../../../cat-zap/res/powerblast.png", 48, 48, SPRITE_POWERBLAST);

	//level1enemies
	loadSprite("../../../cat-zap/res/level1enemies.png", 16, 16, SPRITE_LEVEL1_ENEMIES);


	//numbersprite.surface = IMG_Load("../../../nessnake/art/numbers.png");
	////SDL_SetColorKey(numbersprite.surface, SDL_TRUE, SDL_MapRGB(numbersprite.surface->format, 0, 0, 0));
	//numbersprite.texture = SDL_CreateTextureFromSurface(renderer, numbersprite.surface);
}


// externs
const u8 gamebgMapBank, gamebgMap[1];
const u8 gamebgTileBank, gamebgTile[1];
const u8 gamebg0MapBank, gamebg0Map[1];
const u8 gamebg0TileBank, gamebg0Tile[1];
const u8 gamebg2MapBank, gamebg2Map[1];
const u8 gamebg2TileBank, gamebg2Tile[1];
const u8 gamebg4MapBank, gamebg4Map[1];
const u8 gamebg4TileBank, gamebg4Tile[1];
const u8 gamebg5MapBank, gamebg5Map[1];
const u8 gamebg5TileBank, gamebg5Tile[1];
const u8 gamebg5tilesTileBank, gamebg5tilesTile[1];
const u8 gamebg8MapBank, gamebg8Map[1];
const u8 gamebg8TileBank, gamebg8Tile[1];
const u8 gamefgMapBank, gamefgMap[1];
const u8 gamefgTileBank, gamefgTile[1];
const u8 rattungMergedMapBank, rattungMergedMap[1];
const u8 rattungTileBank, rattungTile[1];
const u8 alphabetBGTileBank, alphabetBGTile[1];
const u8 alphabetTileBank, alphabetTile[1];
const u8 spritesTileBank, spritesTile[1];
const u8 flamethrowerTileBank, flamethrowerTile[1];
const u8 weaponiconsTileBank, weaponiconsTile[1];
const u8 railgunTileBank, railgunTile[1];
const u8 powerblastTileBank, powerblastTile[1];
const u8 satelliteTileBank, satelliteTile[1];
const u8 level1enemiesTileBank, level1enemiesTile[1];
const u8 level2enemiesTileBank, level2enemiesTile[1];
const u8 level3enemiesTileBank, level3enemiesTile[1];
const u8 level4enemiesTileBank, level4enemiesTile[1];
const u8 level5enemiesTileBank, level5enemiesTile[1];
const u8 level5enemies2TileBank, level5enemies2Tile[1];
const u8 level6enemiesTileBank, level6enemiesTile[1];
const u8 level7enemiesTileBank, level7enemiesTile[1];
const u8 spaceserpentTileBank, spaceserpentTile[1];
const u8 alphacawTileBank, alphacawTile[1];
const u8 rattungspriteTileBank, rattungspriteTile[1];
const u8 rattungsprite2TileBank, rattungsprite2Tile[1];
const u8 swarmcloakTileBank, swarmcloakTile[1];
const u8 rattaswappersTileBank, rattaswappersTile[1];
const u8 base0TileBank;
const u8 base0Tile[1];
const u8 base1TileBank;
const u8 base1Tile[1];
const u8 base2TileBank;
const u8 base2Tile[1];
const u8 base3TileBank;
const u8 base3Tile[1];
const u8 base4TileBank;
const u8 base4Tile[1];
const u8 base5TileBank;
const u8 base5Tile[1];
const u8 baseemptyTileBank;
const u8 baseemptyTile[1];
char music1[1];
char music2[1];
char music3[1];
char music4[1];
char music5[1];
char music6[1];
char music7[1];
char music8[1];

const u8 mode7MergedBank;
const u8 mode7Merged[1];
const u8 gameoverbgMapBank, gameoverbgMap[1];
const u8 gameoverbgTileBank, gameoverbgTile[1];