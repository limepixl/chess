#include "event.hpp"
#include "../display/display.hpp"
#include <glad/glad.h>
#include <SDL.h>
#include <cstdio>
#include "../math/math.hpp"

void ProcessEvents(Display *display, State *state, glm::mat4 &view)
{
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_WINDOWEVENT)
		{
			SDL_WindowEvent we = e.window;
			if(we.event == SDL_WINDOWEVENT_RESIZED)
			{
				SDL_GetWindowSize(display->window, &display->width, &display->height);
				glViewport(0, 0, display->width, display->height);
				display->changedSize = true;
			}
			else if(we.event == SDL_WINDOWEVENT_MAXIMIZED)
			{
				SDL_MaximizeWindow(display->window);
				SDL_GetWindowSize(display->window, &display->width, &display->height);
				glViewport(0, 0, display->width, display->height);
				display->changedSize = true;
			}
		}
		else if(e.type == SDL_QUIT)
		{
			SDL_Quit();
			exit(0);
		}
		else if(e.type == SDL_KEYDOWN)
		{
			switch(e.key.keysym.sym)
			{
				case SDLK_SPACE:
				{
					if(state->shouldRotate == true)
						break;
						
					state->turn = state->turn ? 0 : 1;
					state->shouldRotate = true;
					break;
				}

				default:
				{
					break;
				}
			}
		}
		else if(e.type == SDL_MOUSEBUTTONDOWN)
		{
			switch(e.button.button)
			{
				case SDL_BUTTON_LEFT:
				{
					state->shouldCastRay = true;
					
					break;
				}
			}
		}
	}
}