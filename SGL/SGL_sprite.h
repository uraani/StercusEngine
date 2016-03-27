#pragma once
#include "SGL_texture.h"
#include "SGL_dynamic_renderer.h"
typedef struct _SGL_SpriteVertex
{
	SGL_Vec2 pos, texCoord;
}SGL_SpriteVertex;
typedef struct _SGL_Sprite
{
	U32 visible;
	SGL_Mat3 transform;
	SGL_TexRegion region;
} SGL_Sprite;
typedef struct _SGL_DynamicSpriteRenderer
{
	SGL_Vec2 texSize;
	SGL_Sprite* sprites;
	SGL_DynamicRenderer renderer;
	U32 spriteCount;
	U32 spriteCountMax;
} SGL_DynamicSpriteRenderer;
typedef struct _SGL_RenderContext SGL_RenderContext;
extern SGL_DynamicSpriteRenderer SGL_CreateDynamicSpriteRenderer(U32 spriteCount, U32 texHandle, const SGL_RenderContext* rContext);
extern void SGL_SpriteRendererDrawAll(const SGL_DynamicSpriteRenderer renderer, const SGL_RenderContext* rContext);