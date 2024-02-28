#include "window.hh"

#include "SDL/SDL.h"
#include "../core.hh"
#include "../settings.hh"
#include "input.hh"

#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"

void InitWindow(Window* w)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	w->window = SDL_CreateWindow("CV", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	
	SDL_SetRelativeMouseMode((SDL_bool)true);
	w->relativeMouse = true;
}

extern bool running;

void UpdateWindowEvents(Window* w)
{
	Input::Update();


	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (!w->relativeMouse)
		{ImGui_ImplSDL2_ProcessEvent(&e);}
		
		switch (e.type)
		{
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			if (e.key.repeat == 0)
			{
				Input::SetKeyDown(e.key.keysym.sym);
			}
			else
			{
				Input::SetKeyDownRepeat(e.key.keysym.sym);
			}
			//Logger::Log(std::to_string((int)e.key.keysym.sym) + " pressed");
			break;
		case SDL_KEYUP:
			Input::SetKeyUp(e.key.keysym.sym);
			//Logger::Log(std::to_string((int)e.key.keysym.sym) + " released");
			break;
		case SDL_MOUSEMOTION:
			MousePos mousePos;

			mousePos.x = e.motion.x;
			mousePos.y = e.motion.y;
			mousePos.relativeX = e.motion.xrel;
			mousePos.relativeY = e.motion.yrel;

			if (w->relativeMouse)//if (mouseRelative)
			{
				int x,y;
				SDL_GetWindowSize(w->window, &x, &y);
				SDL_WarpMouseInWindow(w->window, x/2, y/2);
			} else {
				mousePos.relativeX = 0;
				mousePos.relativeY = 0;
			}

			Input::SetMouseMovement(mousePos);
			

			break;
		case SDL_MOUSEBUTTONDOWN:
			Input::SetMouseButton((MouseButton)e.button.button, true);

			break;
		case SDL_MOUSEBUTTONUP:
			Input::SetMouseButton((MouseButton)e.button.button, false);
			
			break;
		}
	}

	if (Input::KeyboardCheckPressed(SDLK_ESCAPE))
	{
		SDL_SetRelativeMouseMode((SDL_bool)!SDL_GetRelativeMouseMode());
		w->relativeMouse = SDL_GetRelativeMouseMode();
	}
}

void FreeWindow(Window* w)
{
	SDL_DestroyWindow(w->window);
	SDL_Quit();
}
