#pragma once

#include <SDL2/SDL.h>

// https://gist.github.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c
int
SDL_RenderDrawCircle(SDL_Renderer * renderer, int x, int y, int radius);

// https://gist.github.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c
int
SDL_RenderFillCircle(SDL_Renderer * renderer, int x, int y, int radius);