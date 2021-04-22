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

	Texture texture = LoadTextureFromFile("res/images/test.png");
	glActiveTexture(GL_TEXTURE0 + texture.index);
	glBindTexture(GL_TEXTURE_2D, texture.ID);
	
	Shader shader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/basicf.glsl");
	glUseProgram(shader.ID);
	glUniform1i(shader.uniforms["tex"], texture.index);

	float vertices[]
	{
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), (void *)vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

	glm::mat4 view(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	glUniformMatrix4fv(shader.uniforms["view"], 1, GL_FALSE, &view[0][0]);
	
	while(true)
	{
		ProcessEvents(&display);

		if(display.changedSize)
		{
			glm::mat4 projection = GetProjectionMatrix(&display);
			glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
			
			display.changedSize = false;
		}

		glm::mat4 model(1.0f);
		model = glm::rotate(model, glm::radians((float)SDL_GetTicks() / 2.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(shader.uniforms["model"], 1, GL_FALSE, &model[0][0]);

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		SDL_GL_SwapWindow(display.window);
	}

	SDL_Quit();
	return 0;
}