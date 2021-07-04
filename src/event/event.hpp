#pragma once
#include <glm/mat4x4.hpp>

struct State
{
	int turn; // 0 for white 1 for black
	bool shouldRotate;
	bool shouldCastRay;
	
	struct Entity *selectedEntity;
	struct Entity *lastMove;
	glm::vec2 lastMoveEmpty;
	unsigned long ticks;
    
	int grid[64];
    
	bool whiteCheck = false;
	bool blackCheck = false;
    
    bool whiteCanCastle = true;
    bool blackCanCastle = true;
};

void ProcessEvents(struct Display *display, State *state);