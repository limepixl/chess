#pragma once
#include <glm/mat4x4.hpp>

struct State
{
	int turn; // 1 for white 0 for black
	bool shouldRotate;
	bool shouldCastRay;
	struct Entity *selectedEntity;
};

void ProcessEvents(struct Display *display, State *state);