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

	Shader shader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/basicf.glsl");
	glUseProgram(shader.ID);

	Mesh rook = LoadMeshFromOBJ("res/models/rook-moved.obj");
	Mesh queen = LoadMeshFromOBJ("res/models/queen-moved.obj");
	Mesh pawn = LoadMeshFromOBJ("res/models/pawn-moved.obj");
	Mesh knight = LoadMeshFromOBJ("res/models/knight-moved.obj");
	Mesh king = LoadMeshFromOBJ("res/models/king-moved.obj");
	Mesh bishop = LoadMeshFromOBJ("res/models/bishop-moved.obj");
	
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

		glm::mat4 view(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -20.0f));
		view = glm::rotate(view, glm::radians((float)SDL_GetTicks() / 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(shader.uniforms["view"], 1, GL_FALSE, &view[0][0]);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(shader.uniforms["model"], 1, GL_FALSE, &model[0][0]);
		glBindVertexArray(king.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)king.numVertices);

		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(shader.uniforms["model"], 1, GL_FALSE, &model[0][0]);
		glBindVertexArray(queen.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)queen.numVertices);

		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(shader.uniforms["model"], 1, GL_FALSE, &model[0][0]);
		glBindVertexArray(knight.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)knight.numVertices);

		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(shader.uniforms["model"], 1, GL_FALSE, &model[0][0]);
		glBindVertexArray(bishop.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)bishop.numVertices);

		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(shader.uniforms["model"], 1, GL_FALSE, &model[0][0]);
		glBindVertexArray(rook.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)rook.numVertices);

		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(shader.uniforms["model"], 1, GL_FALSE, &model[0][0]);
		glBindVertexArray(pawn.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)pawn.numVertices);

		SDL_GL_SwapWindow(display.window);
	}

	SDL_Quit();
	return 0;
}