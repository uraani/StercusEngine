#pragma once
#include "SDL.h"
#include "SGL_window.h"
extern int SGL_Init(void);
extern SGL_Window* SGL_CreateWindow(const char* title, int GLMajorVersion, int GLMinorVersion, int x, int y, int w, int h, Uint32 SDLflags);
extern void SGL_Quit(void);
