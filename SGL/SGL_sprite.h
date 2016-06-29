#pragma once
#include "SGL_texture.h"
#include "SGL_dynamic_renderer.h"
#define MAX_SPRITES 16384
typedef struct _SGL_StaticRenderer
{
	SGL_Vec2 texSize;
	SGL_Mesh mesh;
	SGL_VAO VAO;
	U32 locked;
	U32 texHandle;
	U32 shaderHandle;
	U32 spriteCount;
	U32 spriteCountMax;
	U32 renderType;
} SGL_StaticRenderer;
typedef struct _SGL_DynamicRenderer
{
	GLsync syncs[SGL_BUFFER_COUNT_MAX];
	SGL_Vec2 texSize;
	SGL_Mesh mesh;
	SGL_VAO VAO;
	U32 maxOffset;
	U32 bufferOffset;
	U32 texHandle;
	U32 shaderHandle;
	U32 spriteCount[SGL_BUFFER_COUNT_MAX];
	U32 spriteCountMax;
	U32 renderType;
} SGL_DynamicRenderer;
typedef struct _SGL_SpriteVertex
{
	SGL_Vec2 pos, uvs;
} SGL_SpriteVertex;
typedef struct _SGL_ColorVertex
{
	SGL_Vec2 pos;
	SGL_Color color;
} SGL_ColorVertex;
typedef struct _SGL_ColoredSpriteVertex
{
	SGL_Vec2 pos;
	SGL_Color color;
	SGL_Vec2 uvs;
} SGL_ColoredSpriteVertex;
typedef struct _SGL_PointSpriteData
{
	SGL_Vec2 pos;
	SGL_Vec3 uvs;
} SGL_PointSpriteData;
typedef struct _SGL_ColorRectData
{
	SGL_ColorVertex verts[4];
} SGL_ColorRectData;
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
typedef struct _SGL_RenderContext SGL_RenderContext;
extern SGL_DynamicRenderer SGL_CreateSectorRenderer(const SGL_RenderContext* rContext);
extern SGL_DynamicRenderer SGL_CreatePointSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D* tex, const SGL_RenderContext* rContext);
extern SGL_DynamicRenderer SGL_CreatePointRenderer(U32 spriteCountMax, const U32 shaderEnum, const SGL_RenderContext* rContext);
extern SGL_DynamicRenderer SGL_CreateSimpleSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D* tex, const SGL_RenderContext* rContext);
extern SGL_StaticRenderer SGL_CreateStaticSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D* tex, const SGL_RenderContext* rContext);
extern SGL_StaticRenderer SGL_CreateStaticColorRenderer(U32 spriteCountMax, U32 shader, const SGL_RenderContext* rContext);
inline void WaitBuffer(GLsync gSync)
{
	for (;;)
	{
		GLenum waitReturn = glClientWaitSync(gSync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
		if (waitReturn == GL_ALREADY_SIGNALED || waitReturn == GL_CONDITION_SATISFIED)
		{
			return;
		}
	}
}
inline void SGL_RendererSync(GLsync sync)
{
	WaitBuffer(sync);
}
inline void SGL_StaticColorRendererCommit(SGL_StaticRenderer* scr)
{
	glBindBuffer(GL_ARRAY_BUFFER, scr->VAO.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SGL_ColorRectData) * scr->spriteCountMax, scr->mesh.vertexData, GL_STATIC_DRAW);
	SDL_free(scr->mesh.vertexData);
}
inline void SGL_StaticSpriteRendererCommit(SGL_StaticRenderer* ssr)
{
	glBindBuffer(GL_ARRAY_BUFFER, ssr->VAO.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SGL_SpriteData) * ssr->spriteCountMax, ssr->mesh.vertexData, GL_STATIC_DRAW);
	SDL_free(ssr->mesh.vertexData);
}
inline void SGL_AddPointSpritePS(SGL_DynamicRenderer* psr, SGL_Vec2 pos, SGL_SquareTexRegion reg)
{
	SDL_assert(psr->spriteCount[psr->bufferOffset] < psr->spriteCountMax);
	SGL_PointSpriteData* vertexData = (SGL_PointSpriteData*)psr->mesh.vertexData;
	size_t spriteOffset = psr->spriteCountMax*psr->bufferOffset + psr->spriteCount[psr->bufferOffset];
	vertexData[spriteOffset].pos = pos;
	vertexData[spriteOffset].uvs.x = reg.offset.x / psr->texSize.y;
	vertexData[spriteOffset].uvs.y = reg.offset.x / psr->texSize.y;
	vertexData[spriteOffset].uvs.z = reg.size/2.0f;
	psr->spriteCount[psr->bufferOffset]++;
}
inline void SGL_AddPointPC(SGL_DynamicRenderer* pr, SGL_Vec2 pos, SGL_Color col)
{
	SDL_assert(pr->spriteCount[pr->bufferOffset] < pr->spriteCountMax);
	SGL_ColorVertex* vertexData = (SGL_ColorVertex*)pr->mesh.vertexData;
	size_t spriteOffset = pr->spriteCountMax*pr->bufferOffset + pr->spriteCount[pr->bufferOffset];
	vertexData[spriteOffset].pos = pos;
	vertexData[spriteOffset].color = col;
	pr->spriteCount[pr->bufferOffset]++;
}
inline void SGL_AddStaticSpriteMask(SGL_StaticRenderer* scr)
{
	SDL_assert(scr->spriteCount < scr->spriteCountMax);
	const SGL_Vec2 quadPosition[] =
	{
		{ 1.0f, 1.0f },
		{ -1.0f, 1.0f },
		{ -1.0f,-1.0f },
		{ 1.0f,-1.0f },
	};
	SGL_SpriteData* vertexData = (SGL_SpriteData*)scr->mesh.vertexData;
	size_t spriteOffset = scr->spriteCount;
	vertexData[spriteOffset].verts[0].pos.x =  1.0f;
	vertexData[spriteOffset].verts[0].pos.y =  1.0f;
	vertexData[spriteOffset].verts[0].uvs.x =  1.0f;
	vertexData[spriteOffset].verts[0].uvs.y =  1.0f;
	vertexData[spriteOffset].verts[1].pos.x = -1.0f;
	vertexData[spriteOffset].verts[1].pos.y =  1.0f;
	vertexData[spriteOffset].verts[1].uvs.x =  0.0f;
	vertexData[spriteOffset].verts[1].uvs.y =  1.0f;
	vertexData[spriteOffset].verts[2].pos.x = -1.0f;
	vertexData[spriteOffset].verts[2].pos.y = -1.0f;
	vertexData[spriteOffset].verts[2].uvs.x =  0.0f;
	vertexData[spriteOffset].verts[2].uvs.y =  0.0f;
	vertexData[spriteOffset].verts[3].pos.x =  1.0f;
	vertexData[spriteOffset].verts[3].pos.y = -1.0f;
	vertexData[spriteOffset].verts[3].uvs.x =  1.0f;
	vertexData[spriteOffset].verts[3].uvs.y =  0.0f;
	scr->spriteCount++;
}
inline void SGL_AddStaticColoredRectanglePS(SGL_StaticRenderer* scr, SGL_Vec2 pos, SGL_Vec2 size, SGL_Color* colors)
{
	SDL_assert(scr->spriteCount < scr->spriteCountMax);
	const SGL_Vec2 quadPosition[] =
	{
		{ 0.5f, 0.5f },
		{ -0.5f, 0.5f },
		{ -0.5f,-0.5f },
		{ 0.5f,-0.5f },
	};
	SGL_ColorRectData* vertexData = (SGL_ColorRectData*)scr->mesh.vertexData;
	const U32 assddsa = sizeof(SGL_ColorRectData);
	size_t spriteOffset = scr->spriteCount;
	vertexData[spriteOffset].verts[0].pos.x = quadPosition[0].x * size.x + pos.x;
	vertexData[spriteOffset].verts[0].pos.y = quadPosition[0].y * size.y + pos.y;
	vertexData[spriteOffset].verts[0].color = colors[0];
	vertexData[spriteOffset].verts[1].pos.x = quadPosition[1].x * size.x + pos.x;
	vertexData[spriteOffset].verts[1].pos.y = quadPosition[1].y * size.y + pos.y;
	vertexData[spriteOffset].verts[1].color = colors[1];
	vertexData[spriteOffset].verts[2].pos.x = quadPosition[2].x * size.x + pos.x;
	vertexData[spriteOffset].verts[2].pos.y = quadPosition[2].y * size.y + pos.y;
	vertexData[spriteOffset].verts[2].color = colors[2];
	vertexData[spriteOffset].verts[3].pos.x = quadPosition[3].x * size.x + pos.x;
	vertexData[spriteOffset].verts[3].pos.y = quadPosition[3].y * size.y + pos.y;
	vertexData[spriteOffset].verts[3].color = colors[3];
	scr->spriteCount++;
}
inline void SGL_AddStaticSpritePS(SGL_StaticRenderer* ssr, SGL_Vec2 pos, SGL_Vec2 size, SGL_TexRegion reg)
{
	SDL_assert(ssr->spriteCount < ssr->spriteCountMax);
	const SGL_Vec2 quadPosition[] =
	{
		{ 0.5f, 0.5f },
		{ -0.5f, 0.5f },
		{ -0.5f,-0.5f },
		{ 0.5f,-0.5f },
	};
	SGL_SpriteData* vertexData = (SGL_SpriteData*)ssr->mesh.vertexData;
	SGL_Vec4 uvs = SM_CalculateUVs(&reg, &ssr->texSize);
	size_t spriteOffset = ssr->spriteCount;
	vertexData[spriteOffset].verts[0].pos.x = quadPosition[0].x * size.x + pos.x;
	vertexData[spriteOffset].verts[0].pos.y = quadPosition[0].y * size.y + pos.y;
	vertexData[spriteOffset].verts[0].uvs.x = uvs.f[2];
	vertexData[spriteOffset].verts[0].uvs.y = uvs.f[3];
	vertexData[spriteOffset].verts[1].pos.x = quadPosition[1].x * size.x + pos.x;
	vertexData[spriteOffset].verts[1].pos.y = quadPosition[1].y * size.y + pos.y;
	vertexData[spriteOffset].verts[1].uvs.x = uvs.f[0];
	vertexData[spriteOffset].verts[1].uvs.y = uvs.f[3];
	vertexData[spriteOffset].verts[2].pos.x = quadPosition[2].x * size.x + pos.x;
	vertexData[spriteOffset].verts[2].pos.y = quadPosition[2].y * size.y + pos.y;
	vertexData[spriteOffset].verts[2].uvs.x = uvs.f[0];
	vertexData[spriteOffset].verts[2].uvs.y = uvs.f[1];
	vertexData[spriteOffset].verts[3].pos.x = quadPosition[3].x * size.x + pos.x;
	vertexData[spriteOffset].verts[3].pos.y = quadPosition[3].y * size.y + pos.y;
	vertexData[spriteOffset].verts[3].uvs.x = uvs.f[2];
	vertexData[spriteOffset].verts[3].uvs.y = uvs.f[1];
	ssr->spriteCount++;
}
inline void SGL_AddSpritePS(SGL_DynamicRenderer* ssr, SGL_Vec2 pos, SGL_Vec2 size, SGL_TexRegion reg)
{
	SDL_assert(ssr->spriteCount[ssr->bufferOffset] < ssr->spriteCountMax);
	const SGL_Vec2 quadPosition[] =
	{
		{ 0.5f, 0.5f },
		{ -0.5f, 0.5f },
		{ -0.5f,-0.5f },
		{ 0.5f,-0.5f },
	};
	SGL_SpriteData* vertexData = (SGL_SpriteData*)ssr->mesh.vertexData;
	SGL_Vec4 uvs = SM_CalculateUVs(&reg, &ssr->texSize);
	size_t spriteOffset = ssr->spriteCountMax*ssr->bufferOffset + ssr->spriteCount[ssr->bufferOffset];
	vertexData[spriteOffset].verts[0].pos.x = quadPosition[0].x * size.x + pos.x;
	vertexData[spriteOffset].verts[0].pos.y = quadPosition[0].y * size.y + pos.y;
	vertexData[spriteOffset].verts[0].uvs.x = uvs.f[2];
	vertexData[spriteOffset].verts[0].uvs.y = uvs.f[3];
	vertexData[spriteOffset].verts[1].pos.x = quadPosition[1].x * size.x + pos.x;
	vertexData[spriteOffset].verts[1].pos.y = quadPosition[1].y * size.y + pos.y;
	vertexData[spriteOffset].verts[1].uvs.x = uvs.f[0];
	vertexData[spriteOffset].verts[1].uvs.y = uvs.f[3];
	vertexData[spriteOffset].verts[2].pos.x = quadPosition[2].x * size.x + pos.x;
	vertexData[spriteOffset].verts[2].pos.y = quadPosition[2].y * size.y + pos.y;
	vertexData[spriteOffset].verts[2].uvs.x = uvs.f[0];
	vertexData[spriteOffset].verts[2].uvs.y = uvs.f[1];
	vertexData[spriteOffset].verts[3].pos.x = quadPosition[3].x * size.x + pos.x;
	vertexData[spriteOffset].verts[3].pos.y = quadPosition[3].y * size.y + pos.y;
	vertexData[spriteOffset].verts[3].uvs.x = uvs.f[2];
	vertexData[spriteOffset].verts[3].uvs.y = uvs.f[1];
	ssr->spriteCount[ssr->bufferOffset]++;
}
inline void SGL_AddSpriteM3(SGL_DynamicRenderer* ssr, SGL_Mat3* mat3, SGL_TexRegion reg)
{
	SDL_assert(ssr->spriteCount[ssr->bufferOffset] < ssr->spriteCountMax);
	//this has to replaced at some point
	const SGL_Vec2 quadPosition[] =
	{									   
		{  0.5f,   0.5f },
		{ -0.5f,   0.5f },
		{ -0.5f,  -0.5f },
		{  0.5f,  -0.5f },
	};
	SGL_SpriteData* vertexData = (SGL_SpriteData*)ssr->mesh.vertexData;
	SGL_Vec4 uvs = SM_CalculateUVs(&reg, &ssr->texSize);
	size_t spriteOffset = ssr->spriteCountMax*ssr->bufferOffset + ssr->spriteCount[ssr->bufferOffset];
	vertexData[spriteOffset].verts[0].pos = SM_M3V2Multiply(mat3, quadPosition[0]);
	vertexData[spriteOffset].verts[0].uvs.x = uvs.f[2];
	vertexData[spriteOffset].verts[0].uvs.y = uvs.f[3];
	vertexData[spriteOffset].verts[1].pos = SM_M3V2Multiply(mat3, quadPosition[1]);
	vertexData[spriteOffset].verts[1].uvs.x = uvs.f[0];
	vertexData[spriteOffset].verts[1].uvs.y = uvs.f[3];
	vertexData[spriteOffset].verts[2].pos = SM_M3V2Multiply(mat3, quadPosition[2]);
	vertexData[spriteOffset].verts[2].uvs.x = uvs.f[0];
	vertexData[spriteOffset].verts[2].uvs.y = uvs.f[1];
	vertexData[spriteOffset].verts[3].pos = SM_M3V2Multiply(mat3, quadPosition[3]);
	vertexData[spriteOffset].verts[3].uvs.x = uvs.f[2];
	vertexData[spriteOffset].verts[3].uvs.y = uvs.f[1];
	ssr->spriteCount[ssr->bufferOffset]++;
}
extern void SGL_DrawLightSector(float lightSize, SGL_Vec2 position, SGL_Vec2 direction, F32 angle,const SGL_Tex2D* tex, SGL_DynamicRenderer* renderer, const SGL_RenderContext* rContext);
extern void SGL_MapShadows(float lightSize, U32 texHandle, const SGL_RenderContext* rContext);
extern void SGL_MapSectorShadows(float lightSize, SGL_Vec2 position, SGL_Vec2 direction, F32 angle,const SGL_Tex2D* tex, const SGL_RenderContext * rContext);
extern void SGL_DrawShadows(float lightSize, U32 texHandle, const SGL_RenderContext* rContext);
extern void SGL_StaticSpriteRendererDraw(SGL_StaticRenderer* renderer, const SGL_RenderContext* rContext);
extern void SGL_StaticSpriteRendererDrawRange(SGL_StaticRenderer* renderer, const SGL_RenderContext* rContext, U32 startSprite, U32 count, SGL_Vec2 offset);
extern void SGL_StaticRendererDrawSP(SGL_StaticRenderer* renderer, const SGL_RenderContext* rContext);
extern void SGL_SimpleSpriteRendererDraw(SGL_DynamicRenderer* renderer, const SGL_RenderContext* rContext);
extern void SGL_PointSpriteRendererDraw(SGL_DynamicRenderer* renderer, const SGL_RenderContext* rContext);
extern void SGL_PointRendererDraw(SGL_DynamicRenderer * renderer, const SGL_RenderContext * rContext);
extern void SGL_DestroySSRenderer(SGL_DynamicRenderer* renderer);