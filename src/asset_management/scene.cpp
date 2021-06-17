#include "scene.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../event/event.hpp"

bool operator==(const Entity& left, const Entity& right)
{
	return left.meshIndex == right.meshIndex && 
		   left.position == right.position &&
		   left.rotation == right.rotation &&
		   left.scale == right.scale &&
		   left.side == right.side &&
		   left.tint == right.tint;
}

void DrawScene(Scene *scene, Shader *shader, Shader *ghostShader, State *state)
{
	// Draw all actual pieces
	for(size_t i = 0; i < scene->entities.size(); i++)
	{
		Entity* current = &scene->entities[i];

		if(current->meshIndex == 7)
		{
			current->rotation.y = (float)(0.2f * state->ticks);
			current->position.y = (float)std::sin(0.005 * (double)state->ticks);

			if(state->lastMoveEmpty != glm::vec2(current->position.x, current->position.z) || state->lastMoveEmpty.x == -1)
				continue;
		}

		glm::vec3 translation = current->position;
		glm::vec3 rotation = current->rotation;
		glm::vec3 scale = current->scale;

		glm::mat4 model(1.0f);
		model = glm::translate(model, translation);
		model = glm::rotate(model, glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);

		glUniformMatrix4fv(shader->uniforms["model"], 1, GL_FALSE, &model[0][0]);
		
		glm::vec3 redTint(0.5f, 0.0f, 0.0f);
		glm::vec3 selectedTint(0.0f, 0.0f, 0.4f);
		glm::vec3 lastMoveTint(0.4f, 0.4f, 0.0f);
		glm::vec3 col(-1.0f, -1.0f, -1.0f);
		if(current->side == 0) // white
		{
			col = glm::vec3(1.0f, 1.0f, 1.0f);
			if(current->meshIndex == 4 && state->whiteCheck)
				col += redTint;
		}
		else if(current->side == 1) // black
		{
			col = glm::vec3(0.3f, 0.3f, 0.3f);
			if(current->meshIndex == 4 && state->blackCheck)
				col += redTint;
		}

		// If entity is selected, add highlight
		if(state->selectedEntity != NULL && state->selectedEntity == current)
		{
			if(current->meshIndex == 4 && (current->side == 0 && state->whiteCheck) || (current->side == 1 && state->blackCheck))
				col -= redTint;
			col += selectedTint;
		}

		// If entity is last, add highlight
		if(state->lastMove != NULL && state->lastMove == current)
			col += lastMoveTint;
		
		// If entity is arrow, draw it with lastMoveTint
		if(current->meshIndex == 7)
			col = glm::vec3(1.0f, 1.0f, 0.5f);

		glUniform3fv(shader->uniforms["col"], 1, &col[0]);

		Mesh currentMesh = scene->meshes[current->meshIndex - 1];
		glBindVertexArray(currentMesh.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)currentMesh.numVertices);
		glBindVertexArray(0);
	}

	// For all ghosts
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	glUseProgram(ghostShader->ID);
	for(size_t i = 0; i < scene->ghosts.size(); i++)
	{
		Entity* current = &scene->ghosts[i];
		glm::vec3 translation = current->position;
		glm::vec3 rotation = current->rotation;
		glm::vec3 scale = current->scale * 1.2f;

		glm::mat4 model(1.0f);
		model = glm::translate(model, translation);
		model = glm::rotate(model, glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);

		glUniformMatrix4fv(ghostShader->uniforms["model"], 1, GL_FALSE, &model[0][0]);
		
		glm::vec3 col(-1.0f, -1.0f, -1.0f);
		if(current->side == 0) // white
		{
			col = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		else if(current->side == 1) // black
		{
			col = glm::vec3(0.3f, 0.3f, 0.3f);
		}
		
		glUniform3fv(ghostShader->uniforms["col"], 1, &col[0]);

		Mesh currentMesh = scene->meshes[current->meshIndex - 1];
		glBindVertexArray(currentMesh.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)currentMesh.numVertices);
		glBindVertexArray(0);
	}
	glUseProgram(shader->ID);
	glDisable(GL_BLEND);
}

void DrawAABBs(std::vector<Entity> &entities, std::vector<Mesh> &meshes, Shader *shader)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for(auto &e : entities)
	{
		glm::mat4 model(1.0f);
		model = glm::translate(model, e.position);
		glUniformMatrix4fv(shader->uniforms["model"], 1, GL_FALSE, &model[0][0]);

		glBindVertexArray(meshes[e.meshIndex].AABB_VAO);
		glDrawArrays(GL_TRIANGLES, 0, 108);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void CheckForCheck(State *state, Scene *scene)
{
	int whiteX = -1, whiteY = -1;
	int blackX = -1, blackY = -1;
	for(int i = 0; i < 8; i++)
	for(int j = 0; j < 8; j++)
	{
		// white king
		if(state->grid[8*i + j] == 4)
		{
			whiteX = j;
			whiteY = i;
		}
		// black king
		else if(state->grid[8*i + j] == -4)
		{
			blackX = j;
			blackY = i;
		}
	}

	if(whiteX == -1 || whiteY == -1 || blackX == -1 || blackY == -1)
		printf("ERROR: There is no king!\n");

	for(Entity &entity : scene->entities)
	{
		// if king, skip
		if(entity.meshIndex == 4)
			continue;
		
		scene->ghosts.clear();
		state->selectedEntity = &entity;
		GenerateGhostsOnGrid(state, scene);
		for(Entity &ghost : scene->ghosts)
		{
			int ghostX = int(ghost.position.x / 5.0f);
			int ghostY = int(ghost.position.z / 5.0f);
			if(ghostX == whiteX && ghostY == whiteY)
			{
				printf("WHITE KING UNDER ATTACK\n");
				state->whiteCheck = true;
			}
			else if(ghostX == blackX && ghostY == blackY)
			{
				printf("BLACK KING UNDER ATTACK\n");
				state->blackCheck = true;
			}
		}
	}

	state->whiteCheck = false;
	state->blackCheck = false;
}

void UpdateBoard(glm::vec3 &rayWorld, glm::vec3 &cameraPos, Scene &scene, State &state)
{
	bool hitGhost = false;
	if(scene.ghosts.size() > 0)
	{
		// Check if hit any ghost and if so, move the piece there
		Entity *ghostEntity = NULL;
		hitGhost = RayHit(cameraPos, rayWorld, scene.ghosts, &ghostEntity);
		if(hitGhost)
		{
			int selectedX = (int)(state.selectedEntity->position.x / 5.0f);
			int selectedY = (int)(state.selectedEntity->position.z / 5.0f);
			int newX = (int)(ghostEntity->position.x / 5.0f);
			int newY = (int)(ghostEntity->position.z / 5.0f);

			State copyState = state;
			Scene copyScene = scene;

			// Update state grid
			state.grid[newX + newY * 8] = state.grid[selectedX + selectedY * 8];
			state.grid[selectedX + selectedY * 8] = 0;

			printf("Current: %d %d\n", selectedX, selectedY);
			printf("Eating: %d %d\n", newX, newY);

			glm::vec3 newPos(newX * 5.0f, 0.0f, newY * 5.0f);
			for(size_t i = 0; i < scene.entities.size(); i++)
			{
				Entity &piece = scene.entities[i];
				if(piece.meshIndex == 7)
					continue;

				if(piece.position == newPos)
				{
					printf("DELETING: %f %f %f\n", piece.position.x, piece.position.y, piece.position.z);
					scene.entities.erase(scene.entities.begin() + i);
					i--;
					continue;
				}
				else if(piece.position == glm::vec3(selectedX * 5.0f, 0.0f, selectedY * 5.0f))
				{
					state.lastMoveEmpty = glm::vec2(piece.position.x, piece.position.z);
					state.selectedEntity = &piece;
					state.lastMove = state.selectedEntity;
				}
			}
			state.selectedEntity->position = newPos;
			state.turn = state.turn == 1 ? 0 : 1;
			state.shouldRotate = true;

			CheckForCheck(&state, &scene);
			// If, after the move, the king is [still] in check then don't apply the move
			if((state.turn == 1 && state.whiteCheck) || (state.turn == 0 && state.blackCheck) ||
				(copyState.whiteCheck && state.whiteCheck) || (copyState.blackCheck && state.blackCheck))
			{
				state = copyState;
				scene = copyScene;
			}
			
			scene.ghosts.clear();
			state.selectedEntity = NULL;
		}
	}

	if(!hitGhost)
	{
		Entity *hitEntity = NULL;
		bool hit = RayHit(cameraPos, rayWorld, scene.entities, &hitEntity);
		if(hit)
		{
			if(hitEntity->side == state.turn)
			{
				state.selectedEntity = hitEntity;

				// If there are highlights, clear them
				if (scene.ghosts.size() > 0)
					scene.ghosts.clear();

				GenerateGhostsOnGrid(&state, &scene);
			}
		}
		else
		{
			state.selectedEntity = NULL;
			scene.ghosts.clear();
		}
	}
}

void RotateBoard(State &state, Camera &cam)
{
	float &t = cam.t;
	glm::vec3 &pos = cam.cameraPos;
	glm::vec3 &dir = cam.dir;

	if(state.turn == 1)
	{
		float amount = Lerp(0.0f, 3.141592f, t);
		cam.zRotate = amount;
		cam.xRotate = amount;
	}
	else
	{
		float amount = Lerp(3.141592f, 6.283084f, t);
		cam.zRotate = amount;
		cam.xRotate = amount;
	}
	pos.z = 40.0f * cos(cam.zRotate) + 17.5f;
	pos.x = (40.0f) * sin(cam.xRotate) + 17.5f;

	dir = cam.destination - pos;
	cam.right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), dir);
	cam.up = glm::cross(dir, cam.right);
	cam.view = glm::lookAt(pos, cam.destination, cam.up);
	t += 0.05f;

	if (t >= 1.05f)
	{
		printf("Switched!\n");
		state.shouldRotate = false;
		t = 0.0f;
	}
}

bool RayHit(glm::vec3 &rayOrigin, glm::vec3 &rayDir, std::vector<Entity> &entities, Entity **hit)
{
	float tmin = 0.001f;
	float tmax = 1000.0f;
	float t = tmax;
	float radius = 2.0f;
	for(Entity &e : entities)
	{
		if(e.meshIndex == 7)
			continue;
			
		glm::vec3 center = e.position;
		glm::vec3 oc = rayOrigin - center;
		float a = glm::dot(rayDir, rayDir);
		float c = glm::dot(oc, oc) - radius * radius;
		float b = 2.0f * glm::dot(oc, rayDir);
		float discriminant = b*b - 4*a*c;
		if(discriminant < 0)
			continue;
		else if(discriminant == 0)
		{
			float res = -b / (2.0f * a);
			if(res < t && res > tmin && res < tmax)
			{
				*hit = &e;
				t = res;
			}
		}
		else 
		{
			float t1 = (-b + sqrt(discriminant)) / (2.0f * a);
			float t2 = (-b - sqrt(discriminant)) / (2.0f * a);
			float tmp = t1 < t2 ? t1 : t2;
			if(tmp < t && tmp <= tmax && tmp >= tmin)
			{
				*hit = &e;
				t = tmp;
			}
		}
	}

	if(*hit != NULL)
		return true;

	return false;
}

static inline void AddToVector(std::vector<Entity> &ghosts, Entity *hitEntity, const glm::ivec2 &pos)
{
	glm::vec3 tmpPos(pos.x * 5.0f, 0.0f, pos.y * 5.0f);
	Entity tmp{hitEntity->meshIndex, tmpPos, hitEntity->rotation, hitEntity->scale, hitEntity->side};
	ghosts.push_back(tmp);
}

void GenerateGhostsOnGrid(State *state, Scene *scene)
{
	int selectedX = int(state->selectedEntity->position.x / 5.0f);
	int selectedY = int(state->selectedEntity->position.z / 5.0f);
	bool currentWhite = state->grid[selectedX + selectedY * 8] > 0;
	int *grid = state->grid;
	std::vector<Entity> &ghosts = scene->ghosts;
	Entity *selectedEntity = state->selectedEntity;

	switch(state->selectedEntity->meshIndex)
	{
	case 1: // rook
	{
		// left
		std::vector<glm::ivec2> positions1;
		for(int i = selectedX - 1; i >= 0; i--)
			positions1.emplace_back(i, selectedY);

		std::vector<glm::ivec2> positions2;
		for(int i = selectedX + 1; i < 8; i++)
			positions2.emplace_back(i, selectedY);

		// down
		std::vector<glm::ivec2> positions3;
		for(int i = selectedY - 1; i >= 0; i--)
			positions3.emplace_back(selectedX, i);

		// up
		std::vector<glm::ivec2> positions4;
		for(int i = selectedY + 1; i < 8; i++)
			positions4.emplace_back(selectedX, i);

		std::vector<std::vector<glm::ivec2>> positions {positions1, positions2, positions3, positions4};
		for(auto &posVec : positions)
		{
			for(auto &pos : posVec)
			{
				int index = pos.x + pos.y * 8;
				bool white = grid[index] > 0;

				if(grid[index] == 0)
					AddToVector(ghosts, selectedEntity, pos);
				else
				{
					if(white != currentWhite)
						AddToVector(ghosts, selectedEntity, pos);

					break;
				}
			}
		}

		break;
	}

	case 2: // knight
	{
		std::vector<glm::ivec2> possiblePos
		{
			{selectedX + 2, selectedY + 1},
			{selectedX + 1, selectedY + 2},
			{selectedX - 1, selectedY + 2},
			{selectedX - 2, selectedY + 1},
			{selectedX - 2, selectedY - 1},
			{selectedX - 1, selectedY - 2},
			{selectedX + 1, selectedY - 2},
			{selectedX + 2, selectedY - 1}
		};

		for(glm::ivec2 &pos : possiblePos)
		{
			if(pos.x >= 0 && pos.y >= 0 && pos.x < 8 && pos.y < 8)
			{
				bool white = grid[pos.x + pos.y * 8] > 0;
				if(currentWhite != white || grid[pos.x + pos.y * 8] == 0)
					AddToVector(ghosts, selectedEntity, glm::ivec2(pos.x, pos.y));
			}
		}

		break;
	}

	case 3: // bishop
	{
		std::vector<glm::ivec2> positions1;
		for(int x = selectedX-1, y = selectedY+1; x >= 0 && y < 8; x--, y++)
			positions1.emplace_back(x, y);

		std::vector<glm::ivec2> positions2;
		for(int x = selectedX+1, y = selectedY+1; x < 8 && y < 8; x++, y++)
			positions2.emplace_back(x, y);

		std::vector<glm::ivec2> positions3;
		for(int x = selectedX-1, y = selectedY-1; x >= 0 && y >= 0; x--, y--)
			positions3.emplace_back(x, y);

		std::vector<glm::ivec2> positions4;
		for(int x = selectedX+1, y = selectedY-1; x < 8 && y >= 0; x++, y--)
			positions4.emplace_back(x, y);

		std::vector<std::vector<glm::ivec2>> positions{positions1, positions2, positions3, positions4};
		for(auto &posVec : positions)
		{
			for(auto &pos : posVec)
			{
				int x = pos.x;
				int y = pos.y;
				if(grid[x + y * 8] == 0)
					AddToVector(ghosts, selectedEntity, glm::ivec2(x, y));
				else
				{
					bool white = grid[x + y * 8] > 0;
					if(white != currentWhite)
						AddToVector(ghosts, selectedEntity, glm::ivec2(x, y));

					break;
				}
			}
		}

		break;
	}
	
	case 4: // king
	{
		std::vector<glm::ivec2> possibleMoves
		{
			{selectedX + 1, selectedY}, 
			{selectedX, selectedY + 1}, 
			{selectedX - 1, selectedY}, 
			{selectedX, selectedY - 1},
			{selectedX + 1, selectedY + 1},
			{selectedX + 1, selectedY - 1},
			{selectedX - 1, selectedY + 1},
			{selectedX - 1, selectedY - 1}
		};
		for(glm::ivec2 &pos : possibleMoves)
		{
			if(pos.x >= 0 && pos.y >= 0 && pos.x < 8 && pos.y < 8)
			{
				bool white = grid[pos.x + pos.y * 8] > 0;
				if(currentWhite != white || grid[pos.x + pos.y * 8] == 0)
					AddToVector(ghosts, selectedEntity, pos);
			}
		}
		break;
	}
	
	case 5: // queen
	{
		std::vector<glm::ivec2> positions1;
		std::vector<glm::ivec2> positionsDiag1;
		std::vector<glm::ivec2> positionsDiag2;
		for(int i = selectedX - 1; i >= 0; i--)
		{
			positions1.emplace_back(i, selectedY);
			int yOffset = selectedX - i;
			if(selectedY + yOffset < 8)
				positionsDiag1.emplace_back(i, selectedY + yOffset);
			if(selectedY - yOffset >= 0)
				positionsDiag2.emplace_back(i, selectedY - yOffset);
		}

		std::vector<glm::ivec2> positions2;
		std::vector<glm::ivec2> positionsDiag3;
		std::vector<glm::ivec2> positionsDiag4;
		for(int i = selectedX + 1; i < 8; i++)
		{
			positions2.emplace_back(i, selectedY);
			int yOffset = i - selectedX;
			if(selectedY + yOffset < 8)
				positionsDiag3.emplace_back(i, selectedY + yOffset);
			if(selectedY - yOffset >= 0)
				positionsDiag4.emplace_back(i, selectedY - yOffset);
		}

		std::vector<glm::ivec2> positions3;
		for(int i = selectedY - 1; i >= 0; i--)
			positions3.emplace_back(selectedX, i);

		std::vector<glm::ivec2> positions4;
		for(int i = selectedY + 1; i < 8; i++)
			positions4.emplace_back(selectedX, i);

		std::vector<std::vector<glm::ivec2>> positions {positions1, positions2, positions3, 
														positions4, positionsDiag1, positionsDiag2, 
														positionsDiag3, positionsDiag4};

		for(auto &posVec : positions)
		{
			for(auto &pos : posVec)
			{
				int index = pos.x + pos.y * 8;
				bool white = grid[index] > 0;

				if(grid[index] == 0)
					AddToVector(ghosts, selectedEntity, pos);
				else
				{
					if(white != currentWhite)
						AddToVector(ghosts, selectedEntity, pos);

					break;
				}
			}
		}

		break;
	}
	
	case 6: // pawn
	{
		// TODO: En Passante not implemented

		std::vector<glm::ivec2> possibleMoves;
		int yOffset = selectedEntity->side == 0 ? -1 : 1;

		int y = selectedY + yOffset;
		int x1 = selectedX - 1;
		int x2 = selectedX;
		int x3 = selectedX + 1;
		if(y < 8 && y >= 0)
		{
			// left
			bool white = grid[x1 + y * 8] > 0;
			bool empty = grid[x1 + y * 8] == 0;
			if(x1 >= 0 && ((!empty && white != currentWhite)))
				AddToVector(ghosts, selectedEntity, glm::ivec2(x1, y));

			// forward
			if(grid[x2 + y * 8] == 0)
				AddToVector(ghosts, selectedEntity, glm::ivec2(x2, y));

			// right
			white = grid[x3 + y * 8] > 0;
			empty = grid[x3 + y * 8] == 0;
			if(x3 < 8 && ((!empty && white != currentWhite)))
				AddToVector(ghosts, selectedEntity, glm::ivec2(x3, y));
		}

		// First move
		y = selectedY + yOffset * 2;
		if(y < 8 && y >= 0 && ((currentWhite && selectedY == 6) || (!currentWhite && selectedY == 1)))
		{
			// forward
			if(grid[x2 + y * 8] == 0 && grid[x2 + (y - yOffset) * 8] == 0)
				AddToVector(ghosts, selectedEntity, glm::ivec2(x2, y));
		}

		break;
	}
	}

}