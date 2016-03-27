#pragma once
#include "SGL_types.h"
#define BUFFER_OFFSET(x) (void*)(x) 
enum SGL_ShaderName
{
	SGL_SHADER_COLOR,
	SGL_SHADER_SPRITE,
	SGL_SHADER_COLOREDSPRITE,
	SGL_SHADER_COUNT
};
typedef struct _SGL_Shader
{
	U32 handle;
	//size in 4 byte units
	U32 vertexSize;
	void(*bindFunction)(void* vao, U32 shaderHandle, U32 vertexCount, U32 indexCount, U32 drawType);
	const char* name;
} SGL_Shader;
typedef struct _SGL_ShaderInfo
{
	U32 type;
	const char* source;
} SGL_ShaderInfo;

typedef struct _SGL_RenderContext SGL_RenderContext;
extern U32 SGL_LoadShaders(SGL_RenderContext* context);