#pragma once
#include "SGL_types.h"
#define BUFFER_OFFSET(x) (void*)(x) 
enum SGL_ShaderName
{
	SGL_SHADER_DEBUG,
	SGL_SHADER_COLOR,
	SGL_SHADER_SPRITE,
	SGL_SHADER_OFFSETSPRITE,
	SGL_SHADER_POINTSPRITE,
	SGL_SHADER_COLOREDSPRITE,
	SGL_SHADER_SP_START,
	SGL_SHADER_SP_COLOR = SGL_SHADER_SP_START,
	SGL_SHADER_COUNT,
};
typedef struct _SGL_Shader
{
	//openGL handle
	U32 handle;
	//size in byte units
	U32 vertexSize;
	//function that binds attributes to VAO
	void(*bindFunction)(void* vao, U32 shaderHandle);
	//shaders name
	const char* name;
} SGL_Shader;
typedef struct _SGL_ShaderInfo
{
	//shader stage type
	U32 type;
	const char* source;
} SGL_ShaderInfo;

typedef struct _SGL_RenderContext SGL_RenderContext;
extern U32 SGL_LoadShaders(SGL_RenderContext* context);
extern U32 SGL_CreateProgram(SGL_ShaderInfo* shaders);