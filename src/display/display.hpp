#pragma once

struct Display
{
	struct SDL_Window *window;
	int width, height;
	
	bool changedSize;
};

Display CreateDisplay(const char *title, int width, int height);