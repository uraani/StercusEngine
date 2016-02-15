#pragma once
#include "SGL_camera.h"
typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;
struct SGL_RenderContext
{
	SGL_Camera cameras[SGL_CAMERA_COUNT];
	SGL_Vec2 windowHalfSizef;
	SGL_Vec2i windowSize;
}typedef SGL_RenderContext;
struct SGL_Window
{
	SGL_RenderContext rContext;
	SDL_Window* handle;
	SDL_GLContext glContext;
}typedef SGL_Window;
//typedef struct SGL_Window SGL_Window;
