#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include "spritedef.h"
#include "commontypes.h"

#define NT_UPD_HORZ 0x40
#define NT_UPD_VERT 0x80
#define NT_UPD_EOF 0xff
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

#define OAM_FLIP_V 0x80
#define OAM_FLIP_H 0x40
#define OAM_BEHIND 0x20

#define MAX(x1, x2) ((x1) < (x2) ? (x2) : (x1))
#define MIN(x1, x2) ((x1) < (x2) ? (x1) : (x2))

#define MASK_SPR 0x10
#define MASK_BG 0x08
#define MASK_EDGE_SPR 0x04
#define MASK_EDGE_BG 0x02
#define MASK_TINT_RED 0x20
#define MASK_TINT_BLUE 0x40
#define MASK_TINT_GREEN 0x80
#define MASK_MONO 0x01

#define NAMETABLE_A 0x2000
#define NAMETABLE_B 0x2400
#define NAMETABLE_C 0x2800
#define NAMETABLE_D 0x2c00

#define NULL 0
#define TRUE 1
#define FALSE 0

#define NT_UPD_HORZ 0x40
#define NT_UPD_VERT 0x80
#define NT_UPD_EOF 0xff

// macro to calculate nametable address from X,Y in compile time

#define NTADR_A(x, y) (NAMETABLE_A | (((y) << 5) | (x)))
#define NTADR_B(x, y) (NAMETABLE_B | (((y) << 5) | (x)))
#define NTADR_C(x, y) (NAMETABLE_C | (((y) << 5) | (x)))
#define NTADR_D(x, y) (NAMETABLE_D | (((y) << 5) | (x)))

#define TILE_ATTR_FULL(pal, prio, flipv, fliph, tile) (tile | ((pal) << 10) | ((prio) << 13) | ((fliph) << 14) | ((flipv) << 15))

// macro to get MSB and LSB

#define MSB(x) (((x) >> 8))
#define LSB(x) (((x)&0xff))

extern SDL_Renderer* renderer;

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
void ppu_wait_nmi(){}

unsigned short snesmul(const unsigned char a, const unsigned char b) {
	return a * b;
}
unsigned short snesdiv(const unsigned short a, const unsigned char b) {
	return a / b;
}
unsigned short snesmod(const unsigned short a, const unsigned char b) {
	return a % b;
}

void oam_meta_spr_x(int x, unsigned char y,
	const unsigned char* data, u8 type) {
}

void oam_meta_spr_x_high(int x, unsigned char y,
	const unsigned char* data) {
}

int _sin(unsigned x) {}
int _cos(unsigned x) {}

void delay(unsigned char frames) {}

#include "asset.h"
extern Asset sprites[];
extern Asset bg;
extern Asset hudsprite;
extern Asset numbersprite;
extern const u8* mapsptr;

void renderNumbers(int x, int y, int pal, int value)
{
	SDL_Rect srcrect;
	srcrect.x = value * 8;
	srcrect.y = 0;
	srcrect.w = 8;
	srcrect.h = 8;

	SDL_Rect dstrect;
	dstrect.x = x;
	dstrect.y = y;
	dstrect.w = 8;
	dstrect.h = 8;

	SDL_RenderCopy(renderer, numbersprite.texture, &srcrect, &dstrect);
}

void renderSprite(int x, int y, int type, int flip, int frame)
{
	if (!sprites[type].texture)
		return;

	SDL_Rect srcrect;
	srcrect.x = frame * sprites[type].w;
	srcrect.y = flip ? sprites[type].h : 0;
	srcrect.w = sprites[type].w;
	srcrect.h = sprites[type].h;

	SDL_Rect dstrect;
	dstrect.x = x;
	dstrect.y = y + 1;
	dstrect.w = sprites[type].w;
	dstrect.h = sprites[type].h;

	SDL_RenderCopy(renderer, sprites[type].texture, &srcrect, &dstrect);

}

void renderMap()
{
}

void oam_clear() {}

const Uint8* state;
Uint8 oldstate[SDL_NUM_SCANCODES];

// poll controller in trigger mode, a flag is set only on button down, not hold
// if you need to poll the pad in both normal and trigger mode, poll it in the
// trigger mode for first, then use pad_state
unsigned char pad_trigger(unsigned char pad)
{
	state = SDL_GetKeyboardState(NULL);
	unsigned char triggerState = 0;
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

	memcpy(oldstate, state, sizeof(Uint8) * SDL_NUM_SCANCODES);

	return triggerState;
}

// get previous pad state without polling ports
unsigned char pad_state(unsigned char pad)
{
	return 0;
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

void loadAssets() {
	sprites[0].surface = IMG_Load("../../../cat-zap/res/sprites.png");
	SDL_SetColorKey(sprites[0].surface, SDL_TRUE, SDL_MapRGB(sprites[0].surface->format, 0, 0, 0));
	sprites[0].texture = SDL_CreateTextureFromSurface(renderer, sprites[0].surface);
	sprites[0].w = 16;
	sprites[0].h = 16;

	//numbersprite.surface = IMG_Load("../../../nessnake/art/numbers.png");
	////SDL_SetColorKey(numbersprite.surface, SDL_TRUE, SDL_MapRGB(numbersprite.surface->format, 0, 0, 0));
	//numbersprite.texture = SDL_CreateTextureFromSurface(renderer, numbersprite.surface);
}