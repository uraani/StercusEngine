#pragma once
#ifndef _SGL_h
#define _SGL_h
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
inline void SGL_SetVPMatrix(SGL_Mat4* matrix, const SGL_RenderContext* rContext)
{
	glBindBuffer(GL_UNIFORM_BUFFER, rContext->uniformMatrixHandle);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SGL_Mat4), matrix);
}
inline void SGL_BindCamera(U32 id, SGL_RenderContext* rContext)
{
	rContext->boundCamera = id;
	if (CHECK_FLAGS(rContext->state, SGL_RENDER_STATE_RENDERING))
	{
		SGL_SetVPMatrix(&rContext->cameras[rContext->boundCamera].vPMatrix, rContext);
	}
}
inline void SGL_BindShader(U32 shaderHandle, SGL_RenderContext* rContext)
{
	if (rContext->boundShader != shaderHandle)
	{
		rContext->boundShader = shaderHandle;
		glUseProgram(shaderHandle);
	}
}
#if defined(NDEBUG) || !defined(_DEBUG) 
#define SGL_CHECK_GL_ERROR ((void)(0))
#else
#define SGL_CHECK_GL_ERROR _CheckGLErrors(__FILE__, __LINE__)
inline void _CheckGLErrors(const char *file, int line)
{
	GLenum error = glGetError();
	if (error != 0)
	{
		SDL_Log("OpenGL Error: %i, File: %s, Line: %i", error, file, line);
	}
}
#endif
extern SGL_Window SGL_CreateWindow(const char* title, I32 GLMajorVersion, I32 GLMinorVersion, U32 bufferCount, I32 x, I32 y, I32 w, I32 h, Uint32 SDLflags);
#ifdef __cplusplus
}
#endif
#endif