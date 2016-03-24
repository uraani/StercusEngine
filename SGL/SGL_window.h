#pragma once
#include "SDL_syswm.h"
#include "SDL.h"
#include "SGL_camera.h"
typedef struct SDL_Window SDL_Window;
typedef struct _SGL_GLContext
{
	SGL_Camera cameras[SGL_CAMERA_COUNT];
	SGL_Vec2 windowHalfSizef;
	SGL_Vec2i windowSize;
	void* handle;
}SGL_GLContext;
typedef struct _SGL_Window
{
	SDL_Window* handle;
	SGL_GLContext glContext;
} SGL_Window;
