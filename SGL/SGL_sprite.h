#pragma once
#include "SGL_texture.h"
#include "SGL_dynamic_renderer.h"
typedef struct _SGL_SpriteVertex
{
	SGL_Vec2 pos, texCoord;
}SGL_SpriteVertex;
typedef struct _SGL_SpriteData
{
	SGL_SpriteVertex verts[4];
} SGL_SpriteData;
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
typedef struct _SGL_SimpleSpriteRenderer
{
	SGL_Vec2 texSize;
	SGL_DynamicRenderer renderer;
	U32 spriteCount;
	U32 spriteCountMax;
} SGL_SimpleSpriteRenderer;
typedef struct _SGL_RenderContext SGL_RenderContext;
extern SGL_DynamicSpriteRenderer SGL_CreateDynamicSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D* tex, const SGL_RenderContext* rContext);
extern SGL_SimpleSpriteRenderer SGL_CreateSimpleSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D* tex, const SGL_RenderContext* rContext);
inline void SGL_AddSpritePS(SGL_SimpleSpriteRenderer* ssr, SGL_Vec2 pos, SGL_Vec2 size, SGL_TexRegion reg)
{
	if (ssr->spriteCount < ssr->spriteCountMax)
	{
		const SGL_Vec2 quadPosition[] =
		{
			{ 0.5f, 0.5f },
			{ -0.5f, 0.5f },
			{ -0.5f,-0.5f },
			{ 0.5f,-0.5f },
		};
		SGL_Vec2* vertexData = (SGL_Vec2*)ssr->renderer.mesh.vertexData;
		SGL_Vec4 uvs = SM_CalculateUVs(&reg, &ssr->texSize);
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 0].x = quadPosition[0].x * size.x + pos.x;
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 0].y = quadPosition[0].y * size.y + pos.y;
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 1].x = uvs.f[2];
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 1].y = uvs.f[3];
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 2].x = quadPosition[1].x * size.x + pos.x;
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 2].y = quadPosition[1].y * size.y + pos.y;
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 3].x = uvs.f[0];
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 3].y = uvs.f[3];
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 4].x = quadPosition[2].x * size.x + pos.x;
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 4].y = quadPosition[2].y * size.y + pos.y;
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 5].x = uvs.f[0];
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 5].y = uvs.f[1];
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 6].x = quadPosition[3].x * size.x + pos.x;
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 6].y = quadPosition[3].y * size.y + pos.y;
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 7].x = uvs.f[2];
		vertexData[ssr->spriteCount*(sizeof(SGL_SpriteData) / sizeof(SGL_Vec2)) + 7].y = uvs.f[1];
		ssr->spriteCount++;
	}
}
extern void SGL_SimpleSpriteRendererDraw(SGL_SimpleSpriteRenderer* renderer, const SGL_RenderContext* rContext, float gamma, U32 debug);
extern void SGL_DynamicSpriteRendererDraw(SGL_DynamicSpriteRenderer* renderer, const SGL_RenderContext* rContext);