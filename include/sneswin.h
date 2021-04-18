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

void pal_all(u8 pal, const unsigned short* data) {}

// set bg palette only, data is 16 bytes array
void pal_bg(const char* data) {}

// set spr palette only, data is 16 bytes array
void pal_spr(const char* data) {}

// set a palette entry, index is 0..31
void pal_col(unsigned char index, unsigned char color) {}

void pal_bright(unsigned char bright) {}

void sfx_play(unsigned char sound) {}
void scroll(unsigned int x) {}
void scroll2(unsigned int x, unsigned int y) {}
void music_play(unsigned char bank, const unsigned char* data) {}


unsigned char RAND_SEED_LOW = 0xfd;
unsigned char RAND_SEED_HIGH = 0xfd;
unsigned char r_A = 0;
unsigned char f_C = 0;
unsigned char f_V = 0;
unsigned char f_Z = 0;
unsigned char f_N = 0;

void set_rand(unsigned int seed)
{
	RAND_SEED_LOW = seed & 255;
	RAND_SEED_HIGH = (seed >> 8) & 255;
}

void setZN(unsigned char value)
{
	f_Z = !value;
	f_N = value & 0x80;
}

void adc(unsigned char operand)
{
	//uint16_t sum = r_A + operand + f_C;
	uint16_t sum = r_A + operand;
	if (f_C)
		sum++;
	//Carry forward or UNSIGNED overflow
	f_C = sum & 0x100;
	//SIGNED overflow, would only happen if the sign of sum is
	//different from BOTH the operands
	f_V = (r_A ^ sum) & (operand ^ sum) & 0x80;
	r_A = (unsigned char)sum;
	setZN(r_A);
}

void asl()
{
	auto prev_C = f_C;
	f_C = r_A & 0x80;
	r_A <<= 1;
	//If Rotating, set the bit-0 to the the previous carry
	//r_A = r_A | (prev_C && (op == ROL));
	setZN(r_A);
}

void eor(unsigned char value) {
	r_A ^= value;
	setZN(r_A);
}

//lda < RAND_SEED
//	asl a
//	bcc @1
//	eor #$cf
//
//	@1:
//
//sta < RAND_SEED
//	rts
unsigned char rand1(void) {
	r_A = RAND_SEED_LOW;
	asl();
	if (f_C)
	{
		eor(0xcf);
	}
	RAND_SEED_LOW = r_A;
}

unsigned char rand2(void) {
	r_A = RAND_SEED_HIGH;
	asl();
	if (f_C)
	{
		eor(0xd7);
	}
	RAND_SEED_HIGH = r_A;
}

unsigned char rand8(void) {
	rand1();
	rand2();
	adc(RAND_SEED_LOW);
	return r_A;
	//return rand() & 255;
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

void vram_inc(const unsigned char inc) {}

u8 normalizeType(u8 type)
{
	if (type >= SPRITE_ENEMY_INDEX + ENEMY_CAW_CAW_BOMBER && type <= SPRITE_ENEMY_INDEX + ENEMY_CAW_CAW_FIGHTER)
	{
		return SPRITE_LEVEL1_ENEMIES;
	}
	if ((type >= SPRITE_ENEMY_INDEX + ENEMY_ASTEROID && type <= SPRITE_ENEMY_INDEX + ENEMY_CAW_CAW_SPAWNER_CHILD) || type == SPRITE_ENEMY_INDEX + ENEMY_SMALL_ASTEROID)
	{
		return SPRITE_LEVEL2_ENEMIES;
	}
	else if (type == SPRITE_EXPLOSION_INDEX || type == SPRITE_ENERGYSHIELD)
	{
		return SPRITE_POWERBLAST;
	}
	else if (type == SPRITE_SATELLITE || type == SPRITE_BULLET_ENEMY_INDEX
		|| type == SPRITE_PLAYER_BULLET || type == (SPRITE_EXPLOSION_INDEX + 1)
		|| type == SPRITE_PLAYER_BULLET_RAILGUN
		|| type == SPRITE_PLAYER_BULLET_HOMMING)
	{
		return SPRITE_SPRITES;
	}
	else if ((type >= SPRITE_ENEMY_INDEX + ENEMY_SPACE_SERPENT && type <= SPRITE_ENEMY_INDEX + ENEMY_SPACE_SERPENT_TAIL) || type == SPRITE_BULLET_ENEMY_INDEX + 2)
	{
		return SPRITE_SPACE_SERPENT;
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

void oam_meta_spr(unsigned char x, unsigned char y,
	const unsigned char* data, u8 type) {
	renderSprite(data[0], data[1], x, y, normalizeType(type));
}

void oam_meta_spr_high_pal(unsigned char x, unsigned char y, unsigned char pal, const unsigned char* data, u8 type) {
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
	loadSprite("../../../cat-zap/res/flamethrower.png", 16, 16, SPRITE_PLAYER_BULLET_FLAMETHROWER);


	//level1enemies
	loadSprite("../../../cat-zap/res/level1enemies.png", 16, 16, SPRITE_LEVEL1_ENEMIES);


	//level2enemies
	loadSprite("../../../cat-zap/res/level2enemies.png", 24, 24, SPRITE_LEVEL2_ENEMIES);

	//serpent
	loadSprite("../../../cat-zap/res/spaceserpent.png", 32, 32, SPRITE_SPACE_SERPENT);


	//numbersprite.surface = IMG_Load("../../../nessnake/art/numbers.png");
	////SDL_SetColorKey(numbersprite.surface, SDL_TRUE, SDL_MapRGB(numbersprite.surface->format, 0, 0, 0));
	//numbersprite.texture = SDL_CreateTextureFromSurface(renderer, numbersprite.surface);
}


// externs
u8 gamebg2MapBank, gamebg2Map[1];
u8 gamebg2TileBank, gamebg2Tile[1];
u8 gamebg5MapBank, gamebg5Map[1];
u8 gamebg5TileBank, gamebg5Tile[1];
u8 gamebg5tilesTileBank, gamebg5tilesTile[1];
u8 gamebg8MapBank, gamebg8Map[1];
u8 gamebg8TileBank, gamebg8Tile[1];
u8 gamefgMapBank, gamefgMap[1];
u8 gamefgTileBank, gamefgTile[1];
u8 rattungMergedMapBank, rattungMergedMap[1];
u8 rattungTileBank, rattungTile[1];
u8 alphabetBGTileBank, alphabetBGTile[1];
u8 alphabetTileBank, alphabetTile[1];
u8 spritesTileBank, spritesTile[1];
u8 flamethrowerTileBank, flamethrowerTile[1];
u8 weaponiconsTileBank, weaponiconsTile[1];
u8 railgunTileBank, railgunTile[1];
u8 powerblastTileBank, powerblastTile[1];
u8 satelliteTileBank, satelliteTile[1];
u8 level1enemiesTileBank, level1enemiesTile[1];
u8 level2enemiesTileBank, level2enemiesTile[1];
u8 level3enemiesTileBank, level3enemiesTile[1];
u8 level4enemiesTileBank, level4enemiesTile[1];
u8 level5enemiesTileBank, level5enemiesTile[1];
u8 level5enemies2TileBank, level5enemies2Tile[1];
u8 level6enemiesTileBank, level6enemiesTile[1];
u8 level7enemiesTileBank, level7enemiesTile[1];
u8 spaceserpentTileBank, spaceserpentTile[1];
u8 alphacawTileBank, alphacawTile[1];
u8 rattungspriteTileBank, rattungspriteTile[1];
u8 rattungsprite2TileBank, rattungsprite2Tile[1];
u8 swarmcloakTileBank, swarmcloakTile[1];
u8 rattaswappersTileBank, rattaswappersTile[1];
u8 base0TileBank;
u8 base0Tile[1];
u8 base1TileBank;
u8 base1Tile[1];
u8 base2TileBank;
u8 base2Tile[1];
u8 base3TileBank;
u8 base3Tile[1];
u8 base4TileBank;
u8 base4Tile[1];
u8 base5TileBank;
u8 base5Tile[1];
u8 baseemptyTileBank;
u8 baseemptyTile[1];
char music1[1];
char music2[1];
char music3[1];
char music4[1];
char music5[1];
char music6[1];
char music7[1];
char music8[1];

u8 level1TileBank;
u8 level1Tile[1];
u8 level2TileBank;
u8 level2Tile[1];
u8 level3TileBank;
u8 level3Tile[1];
u8 level4TileBank;
u8 level4Tile[1];
u8 alphabet16TileBank;
u8 alphabet16Tile[1];

u8 mode7MergedBank;
u8 mode7Merged[1];
u8 gameoverbgMapBank, gameoverbgMap[1];
u8 gameoverbgTileBank, gameoverbgTile[1];


u8 highscoreMapBank, highscoreMap[1];
u8 highscoreTileBank, highscoreTile[1];

u8 warningTileBank, warningTile[1];
u8 powerblast2TileBank, powerblast2Tile[1];

u8 gwlTileBank, gwlTile[1];
u8 bigdaddyTileBank, bigdaddyTile[1];


u8 achievementsMapBank, achievementsMap[1];
u8 achievementsTileBank, achievementsTile[1];
u8 achievementsiconsTileBank, achievementsiconsTile[1];