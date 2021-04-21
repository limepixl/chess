#pragma once
#include <SDL.h>

struct Display
{
	SDL_Window *window;
	int width, height;
};

Display CreateDisplay(const char *title, int width, int height);
void ProcessEvents(Display *display);