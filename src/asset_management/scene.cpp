#include "scene.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../event/event.hpp"

void DrawScene(Scene *scene, Shader *shader, Shader *ghostShader, State *state)
{
	// Draw all actual pieces
	for(size_t i = 0; i < scene->entities.size(); i++)
	{
		Entity* current = &scene->entities[i];
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
		
		glm::vec3 col(-1.0f, -1.0f, -1.0f);
		if(current->side == 1) // white
		{
			col = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		else if(current->side == 0) // black
		{
			col = glm::vec3(0.3f, 0.3f, 0.3f);
		}

		// If entity is selected, add highlight
		if(state->selectedEntity != NULL && state->selectedEntity == current)
			col += glm::vec3(0.4f, 0.0f, 0.0f);
		
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
		if(current->side == 1) // white
		{
			col = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		else if(current->side == 0) // black
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

bool RayHit(glm::vec3 &rayOrigin, glm::vec3 &rayDir, std::vector<Entity> &entities, Entity **hit)
{
	float tmin = 0.001f;
	float tmax = 1000.0f;
	float t = tmax;
	float radius = 2.0f;
	for(Entity &e : entities)
	{
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
			{selectedX, selectedY - 1}
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
	}

}