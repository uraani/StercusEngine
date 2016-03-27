#pragma once
#include "SGL_types.h"
typedef struct _SGL_DynamicRenderer
{
	SGL_Mesh mesh;
	SGL_VAO VAO;
	U32 vertexSize;
	U32 texHandle;
	U32 shaderHandle;
	U32 currentIndexCount;
	U32 currentVertexCount;
} SGL_DynamicRenderer;
typedef struct _SGL_Shader SGL_Shader;
extern SGL_DynamicRenderer SGL_CreateDynamicRenderer(const SGL_Shader* shader, U32 texHandle, U32 vertexCount, U32 indexCount);
extern void SGL_DrawDynamicRenderer(U32 indexCount);