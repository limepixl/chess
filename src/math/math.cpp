#include "math.hpp"
#include "../display/display.hpp"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 GetProjectionMatrix(struct Display *display)
{
	glm::mat4 projection = glm::perspective(glm::radians(40.0f), (float)display->width / display->height, 0.1f, 100.0f);
	return projection;
}