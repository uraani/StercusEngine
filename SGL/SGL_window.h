#pragma once
#include "SDL.h"
#include "SDL_syswm.h"
#ifndef SDL_VIDEO_OPENGL
#include "SGL_vk.h"
#endif
#include "SGL_camera.h"
typedef struct SDL_Window SDL_Window;
#ifdef _WIN32
typedef struct SDL_WindowData
{
	SDL_Window *window;
	HWND hwnd;
	HDC hdc;
	HDC mdc;
	HBITMAP hbm;
	WNDPROC wndproc;
	SDL_bool created;
	WPARAM mouse_button_flags;
	SDL_bool initializing;
	SDL_bool expected_resize;
	SDL_bool in_border_change;
	SDL_bool in_title_click;
	SDL_bool focus_click_pending;
	SDL_bool windowed_mode_was_maximized;
	SDL_bool in_window_deactivation;
	struct SDL_VideoData *videodata;
#if SDL_VIDEO_OPENGL_EGL  
	EGLSurface egl_surface;
#endif
} SDL_WindowData;
#endif
typedef void* SDL_GLContext;
typedef struct SGL_Window
{
	SGL_RenderContext rContext;
	SGL_VkContext vkContext;
	SDL_Window* handle;
	//SDL_GLContext glContext;
} SGL_Window;
