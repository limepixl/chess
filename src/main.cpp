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
	Display display = CreateDisplay("Chess Titans - Stefan Ivanovski 196068 - 2021", windowWidth, windowHeight);

	// Load cubemap
	Shader cubemapShader = LoadShadersFromFiles("res/shaders/cubemapv.glsl", "res/shaders/cubemapf.glsl");
	Texture cubemapTexture = LoadCubemapFromFile("res/images/cubemaps/gradient");
	Mesh cubemap = LoadMesh("res/models/cube.obj");

	Shader texturedShader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/texturef.glsl");
	Texture boardTexture = LoadTextureFromFile("res/images/checkerboard.png");
	Mesh board = LoadMesh("res/models/board.obj");
	
	State state{0, false, false, NULL, NULL, {-1, -1}, 0, {0}};

	Scene scene = LoadSceneFromFile("res/scenes/default.txt", &state);
	Shader shader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/basicf.glsl");

	// Add arrows in scene
	for(int x = 0; x < 8; x++)
	for(int y = 0; y < 8; y++)
	{
		scene.entities.push_back({7, glm::vec3(x * 5.0f, 0.0f, y * 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 0, glm::vec3(0.0f, 0.0f, 0.0f)});
	}

	glUseProgram(shader.ID);
	glm::mat4 projection = GetProjectionMatrix(&display);
	glUniformMatrix4fv(shader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

	// Camera stuff
	Camera camera;
	camera.cameraPos = glm::vec3(17.5f, 50.0f, 40.0f + 17.5f);
	camera.destination = glm::vec3(17.5f, 0.0f, 17.5f);
	camera.dir = camera.destination - camera.cameraPos;
	camera.right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), camera.dir);
	camera.up = glm::cross(camera.dir, camera.right);
	camera.view = glm::lookAt(camera.cameraPos, camera.destination, camera.up);

	Shader ghostShader = LoadShadersFromFiles("res/shaders/basicv.glsl", "res/shaders/ghostf.glsl");

	// Counters
	Uint64 start, end;
	double targetFPS = 1000.0 / 60.0;

	// Set lightPos for all shaders
	glUseProgram(shader.ID);
	glUniform3f(shader.uniforms["lightPos"], 17.5f, 30.0f, 17.5f);
	glUseProgram(texturedShader.ID);
	glUniform3f(texturedShader.uniforms["lightPos"], 17.5f, 30.0f, 17.5f);
	glUseProgram(ghostShader.ID);
	glUniform3f(ghostShader.uniforms["lightPos"], 17.5f, 30.0f, 17.5f);

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
			RotateBoard(state, camera);

		glUniform3fv(shader.uniforms["viewPos"], 1, &camera.cameraPos[0]);
		glUniformMatrix4fv(shader.uniforms["view"], 1, GL_FALSE, &camera.view[0][0]);
		DrawScene(&scene, &shader, &ghostShader, &state);

		glUseProgram(texturedShader.ID);
		glUniform3fv(texturedShader.uniforms["viewPos"], 1, &camera.cameraPos[0]);
		glUniformMatrix4fv(texturedShader.uniforms["view"], 1, GL_FALSE, &camera.view[0][0]);
		glUniformMatrix4fv(texturedShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(17.5f, -2.0f, 17.5f));
		glUniformMatrix4fv(texturedShader.uniforms["model"], 1, GL_FALSE, &model[0][0]);

		glUniform1i(texturedShader.uniforms["tex"], boardTexture.index);
		BindTexture(&boardTexture);

		glBindVertexArray(board.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)board.numVertices);
		glBindVertexArray(0);

		// Update ghost matrices
		glUseProgram(ghostShader.ID);
		glUniform3fv(ghostShader.uniforms["viewPos"], 1, &camera.cameraPos[0]);
		glUniformMatrix4fv(ghostShader.uniforms["view"], 1, GL_FALSE, &camera.view[0][0]);
		glUniformMatrix4fv(ghostShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

		if(state.shouldCastRay)
		{
			state.shouldCastRay = false;

			glm::vec3 rayWorld = CastRay(&display, projection, camera.view);
			UpdateBoard(rayWorld, camera.cameraPos, scene, state);
		}

		// Draw cubemap last
		glUseProgram(cubemapShader.ID);

		glUniformMatrix4fv(cubemapShader.uniforms["projection"], 1, GL_FALSE, &projection[0][0]);

		glm::mat4 viewMatrixSansTranslation = glm::mat4(glm::mat3(camera.view));
		glUniformMatrix4fv(cubemapShader.uniforms["view"], 1, GL_FALSE, &viewMatrixSansTranslation[0][0]);

		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(100.0f, 100.0f, 100.0f));
		glUniformMatrix4fv(cubemapShader.uniforms["model"], 1, GL_FALSE, &model[0][0]);

		glActiveTexture(GL_TEXTURE0 + cubemapTexture.index);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture.ID);

		glBindVertexArray(cubemap.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)cubemap.numVertices);

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

		state.ticks = SDL_GetTicks();
	}

	SDL_Quit();
	return 0;
}