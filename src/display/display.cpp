#include "display.hpp"
#include <SDL.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

Display CreateDisplay(const char* title, int width, int height)
{
	if(SDL_Init(SDL_INIT_VIDEO))
	{
		printf("Unable to initialize SDL2!\n");
		exit(-1);
	}
	SDL_Window *window = SDL_CreateWindow(
		title, 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		width, height, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if(!window)
	{
		printf("Failed to create SDL window!\n");
		exit(-1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(!context)
	{
		printf("Failed to create OpenGL context!\n");
		exit(-1);
	}

	// Init GLAD
	if(!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
	{
		printf("Failed to init GLAD with SDL2\n");
		SDL_Quit();
	}
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glViewport(0, 0, width, height);

	return {window, width, height, true};
}

void ProcessEvents(Display *display)
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
	}
}