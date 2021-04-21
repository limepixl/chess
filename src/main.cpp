#include <stdio.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include "display/display.hpp"

int main()
{
	SDL_SetMainReady();

	int windowWidth = 800;
	int windowHeight = 600;
	Display display = CreateDisplay("Chess - Stefan Ivanovski 196068 - 2021", windowWidth, windowHeight);	
	
	while(true)
	{
		ProcessEvents(&display);

		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(display.window);
	}

	SDL_Quit();
	return 0;
}