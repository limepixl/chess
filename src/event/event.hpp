#pragma once

struct State
{
	int turn; // 0 for white 1 for black
	bool shouldRotate;
};

void ProcessEvents(struct Display *display, State *state);