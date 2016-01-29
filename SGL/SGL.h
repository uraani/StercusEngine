#pragma once
#include <SDL.h>
#include <GL\glew.h>
#include <SDL_opengl.h>
#include <png.h>
#include <SDL_image.h>
#include "SGL_window.h"
#include <begin_code.h>
#ifdef __cplusplus
extern "C" {
#endif
extern void SGL_Quit(void);
extern void SGL_DestroyWindow(SGL_Window* window);
extern void SGL_SetWindowIcon(SGL_Window* window, void * pixels);
extern void SGL_ConvertPNGToIconArray(const char* imagePath, const char* fileName);
extern void SGL_RunGLTest(SGL_Window* window);
extern int SGL_Init(void);
extern SGL_Window* SGL_CreateWindow(const char* title, int GLMajorVersion, int GLMinorVersion, int x, int y, int w, int h, Uint32 SDLflags);
#ifdef __cplusplus
}
#endif
#include "close_code.h"