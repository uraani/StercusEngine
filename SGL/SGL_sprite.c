#include "SGL.h"
SGL_DynamicSpriteRenderer SGL_CreateDynamicSpriteRenderer(U32 spriteCountMax, U32 texHandle, const SGL_RenderContext* rContext)
{
	SGL_DynamicSpriteRenderer dsr;
	dsr.renderer = SGL_CreateDynamicRenderer(&rContext->shaders[SGL_SHADER_SPRITE], texHandle, spriteCountMax *4, spriteCountMax *6);
	dsr.spriteCount = spriteCountMax;
	dsr.sprites = SDL_malloc(spriteCountMax*sizeof(SGL_Sprite));
}
void SGL_SpriteRendererDrawAll(const SGL_DynamicSpriteRenderer dsr, const SGL_RenderContext* rContext)
{
	/*
	if (CHECK_FLAGS(rContext->state, SGL_RENDER_STATE_RENDERING))
	{
		const SGL_Vec2 quadPosition[] =
		{
			{  1.0f, 1.0f }, 
			{ -1.0f, 1.0f }, 
			{ -1.0f,-1.0f }, 
			{  1.0f,-1.0f }, 
		};
		const SGL_Vec2 quadTexCoords[] =
		{
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f },
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
		};
		SGL_Vec2* vertexData = (SGL_Vec2*)dsr.renderer.mesh.vertexData;
		for (size_t i = 0; i < dsr.spriteCount; i++)
		{
			vertexData[i*(SDL_arraylen(quadPosition) + SDL_arraylen(quadTexCoords))] = SM_M3V2Multiply(&dsr.sprites[i].transform, &quadPosition[0]);
			vertexData[i*(SDL_arraylen(quadPosition) + SDL_arraylen(quadTexCoords))+2] = SM_M3V2Multiply(&dsr.sprites[i].transform, &quadPosition[1]);
			vertexData[i*(SDL_arraylen(quadPosition) + SDL_arraylen(quadTexCoords))+4] = SM_M3V2Multiply(&dsr.sprites[i].transform, &quadPosition[2]);
			vertexData[i*(SDL_arraylen(quadPosition) + SDL_arraylen(quadTexCoords))+6] = SM_M3V2Multiply(&dsr.sprites[i].transform, &quadPosition[3]);
		}
	}
	*/
	if (CHECK_FLAGS(rContext->state, SGL_RENDER_STATE_RENDERING))
	{
		const SGL_Vec4 quadPosX = { 1.0f,-1.0f,-1.0f, 1.0f };
		const SGL_Vec4 quadPosY = { 1.0f, 1.0f,-1.0f,-1.0f };
		const SGL_Vec4 texSize = { dsr.texSize.x,dsr.texSize.y,dsr.texSize.x,dsr.texSize.y };
		F32* vertexData = (F32*)dsr.renderer.mesh.vertexData;
		for (size_t i = 0; i < dsr.spriteCount; i++)
		{
			SM_M3V2X4MultiplyStride(&dsr.sprites[i].transform, &quadPosX, &quadPosY, &vertexData[sizeof(SGL_SpriteVertex) / sizeof(F32) * 4 * i], sizeof(SGL_SpriteVertex) / sizeof(F32));
			SM_CalculateUVs(&dsr.sprites[i].region, &texSize, &vertexData[sizeof(SGL_SpriteVertex) / sizeof(F32) * 4 * i + sizeof(SGL_Vec2)], sizeof(SGL_SpriteVertex) / sizeof(F32));
			dsr.renderer.mesh.indexData[i * 6] = i * 4 + 0;
			dsr.renderer.mesh.indexData[i * 6 + 1] = i * 4 + 1;
			dsr.renderer.mesh.indexData[i * 6 + 2] = i * 4 + 2;
			dsr.renderer.mesh.indexData[i * 6 + 3] = i * 4 + 2;
			dsr.renderer.mesh.indexData[i * 6 + 4] = i * 4 + 3;
			dsr.renderer.mesh.indexData[i * 6 + 5] = i * 4 + 0;
		}
		glBindBuffer(GL_ARRAY_BUFFER, dsr.renderer.VAO.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, dsr.spriteCount*sizeof(SGL_SpriteVertex) * 4, dsr.renderer.mesh.vertexData);
		SGL_DrawDynamicRenderer(dsr.spriteCount * 6);
	}
}
