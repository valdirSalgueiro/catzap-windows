#pragma once
#include <SDL.h>

typedef struct
{
	SDL_Surface* surface;
	SDL_Texture* texture;
	int w;
	int h;
}Asset;