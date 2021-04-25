#include <stdio.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "display/display.hpp"
#include "asset_management/asset_management.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "math/math.hpp"

int Texture::numTexturesLoaded = 0;

int main()
{
	SDL_SetMainReady();

	int windowWidth = 1280;
	int windowHeight = 720;
	Display display = CreateDisplay("Chess - Stefan Ivanovski 196068 - 2021", windowWidth, windowHeight);

	Scene scene = LoadSceneFromFile("res/scenes/default.txt");
	Shader shader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/basicf.glsl");
	glUseProgram(shader.ID);

	// Camera stuff
	glm::vec3 cameraPos = glm::vec3(0.0f, 30.0f, 50.0f);
	glm::vec3 destination(0.0f, 0.0f, 0.0f);
	glm::vec3 dir = destination - cameraPos;
	glm::vec3 right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), dir);
	glm::vec3 up = glm::cross(dir, right);
	glm::mat4 view = glm::lookAt(cameraPos, destination, up);
	glUniformMatrix4fv(shader.uniforms["view"], 1, GL_FALSE, &view[0][0]);

	while(true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		ProcessEvents(&display);

		if(display.changedSize)
		{
			glm::mat4 projection = GetProjectionMatrix(&display);
			glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
			
			display.changedSize = false;
		}

		DrawScene(&scene, &shader);

		SDL_GL_SwapWindow(display.window);
	}

	SDL_Quit();
	return 0;
}