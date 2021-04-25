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

	Shader texturedShader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/texturef.glsl");
	Texture boardTexture = LoadTextureFromFile("res/images/test.png");
	Mesh board = LoadMeshFromOBJ("res/models/board.obj");
	
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

	glm::mat4 projection = GetProjectionMatrix(&display);
	glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

	while(true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		ProcessEvents(&display);

		glUseProgram(shader.ID);
		if(display.changedSize)
		{
			projection = GetProjectionMatrix(&display);
			glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
			
			display.changedSize = false;
		}

		DrawScene(&scene, &shader);

		glUseProgram(texturedShader.ID);
		glUniformMatrix4fv(texturedShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(texturedShader.uniforms["view"], 1, GL_FALSE, &view[0][0]);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		glUniformMatrix4fv(texturedShader.uniforms["model"], 1, GL_FALSE, &model[0][0]);

		glUniform1i(texturedShader.uniforms["tex"], boardTexture.index);
		glActiveTexture(GL_TEXTURE0 + boardTexture.index);
		glBindTexture(GL_TEXTURE_2D, boardTexture.ID);

		glBindVertexArray(board.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)board.numVertices);
		glBindVertexArray(0);

		SDL_GL_SwapWindow(display.window);
	}

	SDL_Quit();
	return 0;
}