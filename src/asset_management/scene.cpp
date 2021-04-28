#include "scene.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>
#include "../event/event.hpp"

void DrawScene(Scene *scene, Shader *shader, State *state)
{
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
		if(current->side == 0) // white
		{
			col = glm::vec3(1.0f, 1.0f, 1.0f);
		}
		else if(current->side == 1) // black
		{
			col = glm::vec3(0.3f, 0.3f, 0.3f);
		}

		// If entity is selected, add highlight
		if(state->selectedEntity != NULL && state->selectedEntity == current)
			col += glm::vec3(0.4f, 0.0f, 0.0f);
		
		glUniform3fv(shader->uniforms["col"], 1, &col[0]);

		Mesh currentMesh = scene->meshes[current->meshIndex];
		glBindVertexArray(currentMesh.VAO);
		glDrawArrays(GL_TRIANGLES, 0, (int)currentMesh.numVertices);
		glBindVertexArray(0);
	}
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