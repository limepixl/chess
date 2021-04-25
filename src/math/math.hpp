#pragma once

#include <glm/mat4x4.hpp>

glm::mat4 GetProjectionMatrix(struct Display *display);
float Lerp(float v0, float v1, float t);