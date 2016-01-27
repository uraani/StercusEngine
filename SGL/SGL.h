#pragma once
#include "SDL.h"
#include "png.h"
#include "SDL_image.h"
#include "SGL_window.h"
#include "begin_code.h"
#ifdef __cplusplus
extern "C" {
#endif
//extern DECLSPEC int SDLCALL SGL_Init(void);
//extern DECLSPEC SGL_Window* SDLCALL SGL_CreateWindow(const char* title, int GLMajorVersion, int GLMinorVersion, int x, int y, int w, int h, Uint32 SDLflags);
//extern DECLSPEC void SDLCALL SGL_Quit(void);
extern int SGL_Init(void);
extern SGL_Window* SGL_CreateWindow(const char* title, int GLMajorVersion, int GLMinorVersion, int x, int y, int w, int h, Uint32 SDLflags);
extern void SGL_Quit(void);
#ifdef __cplusplus
}
#endif
#include "close_code.h"