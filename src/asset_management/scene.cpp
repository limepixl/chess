#include "scene.hpp"
#include "shader.hpp"
#include "glad/glad.h"
#include <glm/gtc/matrix_transform.hpp>

void DrawScene(Scene *scene, Shader *shader)
{
	for(size_t i = 0; i < scene->entities.size(); i++)
	{
		Entity current = scene->entities[i];
		glm::vec3 translation = current.position;
		glm::vec3 rotation = current.rotation;
		glm::vec3 scale = current.scale;

		glm::mat4 model(1.0f);
		model = glm::translate(model, translation);
		model = glm::rotate(model, glm::radians(rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, scale);

		glUniformMatrix4fv(shader->uniforms["model"], 1, GL_FALSE, &model[0][0]);

		if(current.side == 0) // white
		{
			glUniform3f(shader->uniforms["col"], 1.0f, 1.0f, 1.0f);
		}
		else if(current.side == 1) // black
		{
			glUniform3f(shader->uniforms["col"], 0.3f, 0.3f, 0.3f);
		}
		else // not a piece, just a normal mesh
		{
			glUniform3f(shader->uniforms["col"], -1.0f, -1.0f, -1.0f);
		}
		
		Mesh currentMesh = scene->meshes[current.meshIndex];
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