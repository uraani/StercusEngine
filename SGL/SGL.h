#pragma once

#if defined(ANDROID)
#include "GLES3/gl3.h"
#include "GLES3/gl3ext.h"
#else
#include <GL\glew.h>
#include <SDL_opengl.h>
#endif

//#include <png.h>
#include "SDL_image.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "SGL_window.h"
#include "SGL_data.h"
extern int SGL_Init(void);
extern void SGL_Quit(void);
extern void SGL_DestroyWindow(const SGL_Window* window);
extern void SGL_SetWindowIcon(const SGL_Window* window, void * pixels);
extern void SGL_ConvertPNGToIconArray(const char* imagePath, const char* fileName);
extern void SGL_RunGLTest(const SGL_Window* window);
extern void SGL_StartRender(SGL_Window* rContext);
extern void SGL_EndRender(SGL_Window* rContext);
extern void SGL_BindCamera(SGL_RenderContext* rContext, U32 id);
inline void SGL_BindShader(SGL_RenderContext* rContext, U32 shaderHandle)
{
	if (rContext->boundShader != shaderHandle)
	{
		rContext->boundShader = shaderHandle;
		glUseProgram(shaderHandle);
	}
}
inline void SGL_CheckGLErrors()
{
	GLenum error = glGetError();
	if (error != 0)
	{
		SDL_Log("OpenGL Error: %i", error);
	}
}
extern SGL_Window SGL_CreateWindow(const char* title, int GLMajorVersion, int GLMinorVersion, int x, int y, int w, int h, Uint32 SDLflags);
#ifdef __cplusplus
}
#endif