#include <stdio.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "display/display.hpp"
#include "asset_management/asset_management.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "math/math.hpp"
#include "event/event.hpp"

int Texture::numTexturesLoaded = 0;

int main()
{
	SDL_SetMainReady();

	int windowWidth = 1280;
	int windowHeight = 720;
	Display display = CreateDisplay("Chess - Stefan Ivanovski 196068 - 2021", windowWidth, windowHeight);

	Shader texturedShader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/texturef.glsl");
	Texture boardTexture = LoadTextureFromFile("res/images/checkerboard.png");
	Mesh board = LoadMesh("res/models/board.obj");
	
	State state{0, false, false, NULL, {0}};

	Scene scene = LoadSceneFromFile("res/scenes/default.txt", &state);
	Shader shader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/basicf.glsl");
	glUseProgram(shader.ID);

	glm::mat4 projection = GetProjectionMatrix(&display);
	glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

	// Camera stuff
	// TODO: abstract away somewhere
	glm::vec3 cameraPos = glm::vec3(17.5f, 45.0f, 40.0f + 17.5f);
	glm::vec3 destination(17.5f, 0.0f, 17.5f);
	glm::vec3 dir = destination - cameraPos;
	glm::vec3 right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), dir);
	glm::vec3 up = glm::cross(dir, right);
	glm::mat4 view = glm::lookAt(cameraPos, destination, up);
	float t = 0.0f;
	float xRotate = 0.0f;
	float zRotate = 0.0f;

	Shader outlineShader = LoadShadersFromFiles("res/shaders/outlinev.glsl", "res/shaders/outlinef.glsl");
	Shader ghostShader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/ghostf.glsl");

	// Counters
	Uint64 start, end;
	double targetFPS = 1000.0 / 60.0;

	while(true)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Start counting ms
		start = SDL_GetPerformanceCounter();
		
		ProcessEvents(&display, &state);

		glUseProgram(shader.ID);
		if(display.changedSize)
		{
			projection = GetProjectionMatrix(&display);
			glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
			
			display.changedSize = false;
		}

		if(state.shouldRotate)
		{
			if(state.turn == 1)
			{
				float amount = Lerp(0.0f, 3.141592f, t);
				zRotate = amount;
				xRotate = amount;
			}
			else
			{
				float amount = Lerp(3.141592f, 6.283084f, t);
				zRotate = amount;
				xRotate = amount;
			}
			cameraPos.z = 40.0f * cos(zRotate) + 17.5f;
			cameraPos.x = (40.0f) * sin(xRotate) + 17.5f;

			dir = destination - cameraPos;
			right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), dir);
			up = glm::cross(dir, right);
			view = glm::lookAt(cameraPos, destination, up);
			t += 0.05f;

			if(t >= 1.05f)
			{
				printf("Switched!\n");
				state.shouldRotate = false;
				t = 0.0f;
			}
		}

		glUniformMatrix4fv(shader.uniforms["view"], 1, GL_FALSE, &view[0][0]);
		DrawScene(&scene, &shader, &ghostShader, &state);

		glUseProgram(texturedShader.ID);
		glUniformMatrix4fv(texturedShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(texturedShader.uniforms["view"], 1, GL_FALSE, &view[0][0]);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(17.5f, -2.0f, 17.5f));
		glUniformMatrix4fv(texturedShader.uniforms["model"], 1, GL_FALSE, &model[0][0]);

		glUniform1i(texturedShader.uniforms["tex"], boardTexture.index);
		glActiveTexture(GL_TEXTURE0 + boardTexture.index);
		glBindTexture(GL_TEXTURE_2D, boardTexture.ID);

		glBindVertexArray(board.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)board.numVertices);
		glBindVertexArray(0);

		// Update ghost matrices
		glUseProgram(ghostShader.ID);
		glUniformMatrix4fv(ghostShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(ghostShader.uniforms["view"], 1, GL_FALSE, &view[0][0]);

		// DRAW OUTLINE (TEMP)
		#ifdef AABB
		glUseProgram(outlineShader.ID);
		glUniformMatrix4fv(outlineShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(outlineShader.uniforms["view"], 1, GL_FALSE, &view[0][0]);

		DrawAABBs(scene.entities, scene.meshes, &outlineShader);
		// DRAW OUTLINE (TEMP)
		#endif

		if(state.shouldCastRay)
		{
			state.shouldCastRay = false;

			glm::vec3 rayWorld = CastRay(&display, projection, view);

			Entity *hitEntity = NULL;
			bool hit = RayHit(cameraPos, rayWorld, scene.entities, &hitEntity);
			if(hit)
			{
				state.selectedEntity = hitEntity;

				// If there are highlights, clear them
				if(scene.ghosts.size() > 0)
					scene.ghosts.clear();
				
				// TODO: abstract away into function
				GenerateGhostsOnGrid(&state, &scene);
			}
			else
			{
				state.selectedEntity = NULL;
				if(scene.ghosts.size() > 0)
					scene.ghosts.clear();
			}
		}

		SDL_GL_SwapWindow(display.window);

		// End counting ms and calculate elapsed ms
		end = SDL_GetPerformanceCounter();

		float elapsed = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
		if(elapsed < targetFPS)
		{
			SDL_Delay((Uint32)floor(targetFPS - elapsed));
		}
		end = SDL_GetPerformanceCounter();
		elapsed = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
		// printf("FPS: %.2f\n", 1000.0 / elapsed);
	}

	SDL_Quit();
	return 0;
}