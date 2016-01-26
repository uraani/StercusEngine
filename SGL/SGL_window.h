#pragma once
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;
struct SGL_Window
{
	SDL_Window* window;
	SDL_GLContext context;
}typedef SGL_Window;
//typedef struct SGL_Window SGL_Window;
