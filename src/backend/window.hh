#pragma once

typedef struct SDL_Window SDL_Window;
typedef void *SDL_GLContext;

struct Window
{
	SDL_Window* window;
	SDL_GLContext context;
	bool relativeMouse;
};

void InitWindow(Window* w);
void UpdateWindowEvents(Window* w);
void FreeWindow(Window* w);
