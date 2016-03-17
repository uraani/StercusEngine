#pragma once
#include "SDL_syswm.h"
#ifndef SDL_VIDEO_OPENGL
#include "SGL_vk.h"
#else
#include "SDL.h"
#endif
#include "SGL_camera.h"
typedef struct SDL_Window SDL_Window;
typedef struct _SGL_GLContext
{
	void* handle;
}SGL_GLContext;
typedef struct _SGL_Window
{
	SDL_Window* handle;
	SGL_Vec2 windowHalfSizef;
	SGL_Vec2i windowSize;
#ifndef SDL_VIDEO_OPENGL
	SGL_VkContext vkContext;
	SGL_VkSwapChain vkSwapChain;
#else
	SGL_GLContext glContext;
#endif
	//SDL_GLContext glContext;
} SGL_Window;
