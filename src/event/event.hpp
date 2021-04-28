#pragma once
#include <glm/mat4x4.hpp>

struct State
{
	int turn; // 0 for white 1 for black
	bool shouldRotate;
	bool shouldCastRay;
	struct Entity *selectedEntity;
};

void ProcessEvents(struct Display *display, State *state);