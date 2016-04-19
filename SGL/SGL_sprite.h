#pragma once
#include "SGL_texture.h"
#include "SGL_dynamic_renderer.h"
#define MAX_SPRITES 16384
typedef struct _SGL_SpriteVertex
{
	SGL_Vec2 pos, uvs;
} SGL_SpriteVertex;
typedef struct _SGL_PointSpriteData
{
	SGL_Vec2 pos;
	SGL_Vec3 uvs;
} SGL_PointSpriteData;
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
typedef struct _SGL_SimpleSpriteRenderer
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
} SGL_SimpleSpriteRenderer;
typedef struct _SGL_StaticSpriteRenderer
{
	SGL_Vec2 texSize;
	SGL_Mesh mesh;
	SGL_VAO VAO;
	U32 locked;
	U32 texHandle;
	U32 shaderHandle;
	U32 spriteCount;
	U32 spriteCountMax;
} SGL_StaticSpriteRenderer;
typedef struct _SGL_PointSpriteRenderer
{
	SGL_Vec2 texSize;
	void* vertexData;
	U32 vertexCount;
	SGL_PointVAO VAO;
	U32 maxOffset;
	U32 bufferOffset;
	U32 texHandle;
	U32 shaderHandle;
	GLsync syncs[SGL_BUFFER_COUNT_MAX];
	U32 spriteCount[SGL_BUFFER_COUNT_MAX];
	U32 spriteCountMax;
}SGL_PointSpriteRenderer;
typedef struct _SGL_RenderContext SGL_RenderContext;
extern SGL_PointSpriteRenderer SGL_CreatePointSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D* tex, const SGL_RenderContext* rContext);
extern SGL_SimpleSpriteRenderer SGL_CreateSimpleSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D* tex, const SGL_RenderContext* rContext);
extern SGL_StaticSpriteRenderer SGL_CreateStaticSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D* tex, const SGL_RenderContext* rContext);
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
inline void SGL_StaticSpriteRendererCommit(SGL_StaticSpriteRenderer* ssr)
{
	glBindBuffer(GL_ARRAY_BUFFER, ssr->VAO.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SGL_SpriteData) * ssr->spriteCountMax, ssr->mesh.vertexData, GL_STATIC_DRAW);
	SDL_free(ssr->mesh.vertexData);
}
inline void SGL_AddPointSpritePS(SGL_PointSpriteRenderer* psr, SGL_Vec2 pos, SGL_SquareTexRegion reg)
{
	SDL_assert(psr->spriteCount[psr->bufferOffset] < psr->spriteCountMax);
	SGL_PointSpriteData* vertexData = (SGL_PointSpriteData*)psr->vertexData;
	size_t spriteOffset = psr->spriteCountMax*psr->bufferOffset + psr->spriteCount[psr->bufferOffset];
	vertexData[spriteOffset].pos = pos;
	vertexData[spriteOffset].uvs.x = reg.offset.x / psr->texSize.y;
	vertexData[spriteOffset].uvs.y = reg.offset.x / psr->texSize.y;
	vertexData[spriteOffset].uvs.z = reg.size;
	psr->spriteCount[psr->bufferOffset]++;
}
inline void SGL_AddStaticSpritePS(SGL_StaticSpriteRenderer* ssr, SGL_Vec2 pos, SGL_Vec2 size, SGL_TexRegion reg)
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
inline void SGL_AddSpritePS(SGL_SimpleSpriteRenderer* ssr, SGL_Vec2 pos, SGL_Vec2 size, SGL_TexRegion reg)
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
inline void SGL_AddSpriteM3(SGL_SimpleSpriteRenderer* ssr, SGL_Mat3* mat3, SGL_TexRegion reg)
{
	SDL_assert(ssr->spriteCount[ssr->bufferOffset] < ssr->spriteCountMax);
	const SGL_Vec2 quadPosition[] =
	{									   
		//{ reg.size.x *  0.5f, reg.size.y *  0.5f },
		//{ reg.size.x * -0.5f, reg.size.y *  0.5f },
		//{ reg.size.x * -0.5f, reg.size.y * -0.5f },
		//{ reg.size.x *  0.5f, reg.size.y * -0.5f },
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
extern void SGL_StaticSpriteRendererDraw(SGL_StaticSpriteRenderer* renderer, const SGL_RenderContext* rContext, float opacity);
extern void SGL_SimpleSpriteRendererDraw(SGL_SimpleSpriteRenderer* renderer, const SGL_RenderContext* rContext, float opacity);
extern void SGL_PointSpriteRendererDraw(SGL_PointSpriteRenderer* renderer, const SGL_RenderContext* rContext);
extern void SGL_DestroySSRenderer(SGL_SimpleSpriteRenderer* renderer);