#pragma once
#include "SGL_camera.h"
#include "SGL_shader.h"
#include "SGL_sprite.h"
enum RenderState
{
	SGL_RENDER_STATE_ZERO = 0,
	SGL_RENDER_STATE_RENDERING = 1,
	/*
	SGL_RENDER_STATE_1 = 2,
	SGL_RENDER_STATE_2 = 4,
	SGL_RENDER_STATE_3 = 8,
	SGL_RENDER_STATE_4 = 16,
	SGL_RENDER_STATE_5 = 32,
	SGL_RENDER_STATE_6 = 64,
	SGL_RENDER_STATE_7 = 128,
	SGL_RENDER_STATE_8 = 256,
	SGL_RENDER_STATE_9 = 512,
	SGL_RENDER_STATE_10 = 1024,
	SGL_RENDER_STATE_11 = 2048,
	SGL_RENDER_STATE_12 = 4096,
	SGL_RENDER_STATE_13 = 8192,
	SGL_RENDER_STATE_14 = 16384,
	SGL_RENDER_STATE_15 = 32768,
	*/
};
typedef struct _SGL_RenderContext
{
	SGL_Camera cameras[SGL_CAMERA_COUNT];
	SGL_Shader shaders[SGL_SHADER_COUNT];
	SGL_Vec2 windowHalfSizef;
	SGL_Vec2i windowSize;
	U32 bufferCount;
	U32 state;
	U32 uniformMatrixHandle;
	U32 boundCamera;
	U32 boundShader;
	void* glHandle;
}SGL_RenderContext;
typedef struct SDL_Window SDL_Window;
typedef struct _SGL_Window
{
	SDL_Window* handle;
	SGL_RenderContext rContext;
} SGL_Window;
#define CHECK_FLAGS(state, flags) ((state & flags) == flags)
#define ADD_FLAGS(state, flags) (state |= flags) 
#define REMOVE_FLAGS(state, flags) (state &= ~flags) 
/*
inline U32 CheckFlags(U32 state, U32 flags)
{
	return (state & flags) == flags;
}
inline void AddFlags(U32* state, U32 flags)
{
	*state |= flags;
}
inline void RemoveFlags(U32* state, U32 flags)
{
	*state &= ~flags;
}
*/
