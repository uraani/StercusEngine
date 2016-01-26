#include "SGL.h"

int SGL_Init(void)
{
	return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO);
}

SGL_Window* SGL_CreateWindow(const char* title, int GLMajorVersion, int GLMinorVersion, int x, int y, int w, int h, Uint32 SDLflags)
{
	SDL_DisplayMode targetMode;
	if (SDL_GetCurrentDisplayMode(0, &targetMode))
	{
		return NULL;
	}
	SDL_DisplayMode closestMode;
	SDL_GetClosestDisplayMode(0, &targetMode, &closestMode);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GLMajorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GLMinorVersion);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SGL_Window* window = SDL_malloc(sizeof(SGL_Window));
	window->window = SDL_CreateWindow
		(
			title,
			x, y,
			w, h,
			SDLflags
		);
	return window;
}

void SGL_Quit(void)
{
	SDL_Quit();
}
