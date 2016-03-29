#include "SGL.h"
SGL_DynamicSpriteRenderer SGL_CreateDynamicSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D* tex, const SGL_RenderContext* rContext)
{
	SGL_DynamicSpriteRenderer dsr;
	dsr.renderer = SGL_CreateDynamicRenderer(&rContext->shaders[SGL_SHADER_SPRITE], tex->handle, spriteCountMax *4, spriteCountMax *6);
	dsr.texSize.x = (F32)tex->width;
	dsr.texSize.y = (F32)tex->height;
	dsr.spriteCount = 0;
	dsr.spriteCountMax = spriteCountMax;
	dsr.sprites = SDL_malloc(spriteCountMax*sizeof(SGL_Sprite));
	return dsr;
	
}
SGL_SimpleSpriteRenderer SGL_CreateSimpleSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D * tex, const SGL_RenderContext * rContext)
{
	SGL_SimpleSpriteRenderer ssr;
	ssr.renderer = SGL_CreateDynamicRenderer(&rContext->shaders[SGL_SHADER_SPRITE], tex->handle, spriteCountMax * 4, spriteCountMax * 6);
	ssr.texSize.x = (F32)tex->width;
	ssr.texSize.y = (F32)tex->height;
	ssr.spriteCount = 0;
	ssr.spriteCountMax = spriteCountMax;
	return ssr;
}
void SGL_SimpleSpriteRendererDraw(SGL_SimpleSpriteRenderer* ssr, const SGL_RenderContext * rContext, float gamma, U32 debug)
{
	if (CHECK_FLAGS(rContext->state, SGL_RENDER_STATE_RENDERING))
	{
		for (size_t i = 0; i < ssr->spriteCount; i++)
		{
			ssr->renderer.mesh.indexData[i * 6 + 0] = i * 4 + 0;
			ssr->renderer.mesh.indexData[i * 6 + 1] = i * 4 + 1;
			ssr->renderer.mesh.indexData[i * 6 + 2] = i * 4 + 2;
			ssr->renderer.mesh.indexData[i * 6 + 3] = i * 4 + 2;
			ssr->renderer.mesh.indexData[i * 6 + 4] = i * 4 + 3;
			ssr->renderer.mesh.indexData[i * 6 + 5] = i * 4 + 0;
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, ssr->renderer.VAO.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ssr->spriteCount*sizeof(SGL_SpriteData), ssr->renderer.mesh.vertexData);
	glBindBuffer(GL_ARRAY_BUFFER, ssr->renderer.VAO.EBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, ssr->spriteCount*sizeof(U32) * 6, ssr->renderer.mesh.indexData);
	glBindTexture(GL_TEXTURE_2D, ssr->renderer.texHandle);
	if (debug)
	{
		SGL_DrawDebugDynamicRenderer(rContext, &ssr->renderer, ssr->spriteCount * 6);
	}
	else
	{
		GLint loc = glGetUniformLocation(ssr->renderer.shaderHandle, "gamma");
		if (loc != -1)
		{
			glUniform1f(loc, gamma);
		}
		SGL_DrawDynamicRenderer(rContext, &ssr->renderer, ssr->spriteCount * 6);
	}
	//glBufferData(GL_ARRAY_BUFFER, ssr->spriteCount*sizeof(SGL_SpriteData), NULL, GL_DYNAMIC_DRAW);
	ssr->spriteCount = 0;
}
void SGL_DynamicSpriteRendererDraw(SGL_DynamicSpriteRenderer* dsr, const SGL_RenderContext* rContext)
{
	
	if (CHECK_FLAGS(rContext->state, SGL_RENDER_STATE_RENDERING))
	{
		///*
		{
			U32 timer = SDL_GetTicks();
			const SGL_Vec2 quadPosition[] =
			{
				{  0.5f, 0.5f },
				{ -0.5f, 0.5f },
				{ -0.5f,-0.5f },
				{  0.5f,-0.5f },
			};
			SGL_Vec2* vertexData = (SGL_Vec2*)dsr->renderer.mesh.vertexData;
			U32 targetIndex = 0;
			for (size_t i = 0; i < dsr->spriteCount; i++)
			{
				if (dsr->sprites[i].visible)
				{
					SGL_Vec4 uvs = SM_CalculateUVs(&dsr->sprites[i].region, &dsr->texSize);
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 0] = SM_M3V2Multiply(&dsr->sprites[i].transform, &quadPosition[0]);
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 1].x = uvs.f[2];
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 1].y = uvs.f[3];
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 2] = SM_M3V2Multiply(&dsr->sprites[i].transform, &quadPosition[1]);
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 3].x = uvs.f[0];
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 3].y = uvs.f[3];
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 4] = SM_M3V2Multiply(&dsr->sprites[i].transform, &quadPosition[2]);
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 5].x = uvs.f[0];
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 5].y = uvs.f[1];
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 6] = SM_M3V2Multiply(&dsr->sprites[i].transform, &quadPosition[3]);
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 7].x = uvs.f[2];
					vertexData[targetIndex*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 7].y = uvs.f[1];
					targetIndex++;
				}
			}
			targetIndex = 0;
			for (size_t i = 0; i < dsr->spriteCount; i++)
			{
				if (dsr->sprites[i].visible)
				{
					dsr->renderer.mesh.indexData[targetIndex * 6 + 0] = targetIndex * 4 + 0;
					dsr->renderer.mesh.indexData[targetIndex * 6 + 1] = targetIndex * 4 + 1;
					dsr->renderer.mesh.indexData[targetIndex * 6 + 2] = targetIndex * 4 + 2;
					dsr->renderer.mesh.indexData[targetIndex * 6 + 3] = targetIndex * 4 + 2;
					dsr->renderer.mesh.indexData[targetIndex * 6 + 4] = targetIndex * 4 + 3;
					dsr->renderer.mesh.indexData[targetIndex * 6 + 5] = targetIndex * 4 + 0;
					targetIndex++;
				}
			}
			SDL_Log("ticks spent in simple: %i", SDL_GetTicks() - timer);
			glBindBuffer(GL_ARRAY_BUFFER, dsr->renderer.VAO.VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, dsr->spriteCount*sizeof(SGL_SpriteData), dsr->renderer.mesh.vertexData);
			glBindBuffer(GL_ARRAY_BUFFER, dsr->renderer.VAO.EBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, dsr->spriteCount*sizeof(U32) * 6, dsr->renderer.mesh.indexData);
			//SGL_SpriteVertex* sVert = (SGL_SpriteVertex*)vertexData;
			//for (size_t i = 0; i < 4; i++)
			//{
			//	SDL_Log("Vert:%i, X:%f, Y:%f, U:%f, V:%f", i, sVert[i].pos.x, sVert[i].pos.y, sVert[i].texCoord.x, sVert[i].texCoord.y);
			//}
			SGL_DrawDynamicRenderer(rContext, &dsr->renderer, targetIndex * 6);
		}
		//*/	
		/*
		{
			U32 timer = SDL_GetTicks();
			const SGL_Vec4 quadPosX = { 0.5f,-0.5f,-0.5f, 0.5f };
			const SGL_Vec4 quadPosY = { 0.5f, 0.5f,-0.5f,-0.5f };
			const SGL_Vec4 texSize = { dsr.texSize.x,dsr.texSize.y,dsr.texSize.x,dsr.texSize.y };
			SGL_Vec2* vertexData = (SGL_Vec2*)dsr.renderer.mesh.vertexData;
			for (size_t i = 0; i < dsr.spriteCount; i++)
			{
				SGL_Vec2x4 verts = SM_M3V2X4Multiply(&dsr.sprites[i].transform, &quadPosX, &quadPosY);
				SGL_Vec4 uvs = SM_CalculateUVs(&dsr.sprites[i].region, &texSize);
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2))] = verts.v[0];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 1].x = uvs.f[2];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 1].y = uvs.f[3];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 2] = verts.v[1];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 3].x = uvs.f[0];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 3].y = uvs.f[3];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 4] = verts.v[2];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 5].x = uvs.f[0];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 5].y = uvs.f[1];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 6] = verts.v[3];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 7].x = uvs.f[2];
				vertexData[i*(sizeof(SGL_SpriteData)/sizeof(SGL_Vec2)) + 7].y = uvs.f[1];
			}
			for (size_t i = 0; i < dsr.spriteCount; i++)
			{
				dsr.renderer.mesh.indexData[i * 6] = i * 4 + 0;
				dsr.renderer.mesh.indexData[i * 6 + 1] = i * 4 + 1;
				dsr.renderer.mesh.indexData[i * 6 + 2] = i * 4 + 2;
				dsr.renderer.mesh.indexData[i * 6 + 3] = i * 4 + 2;
				dsr.renderer.mesh.indexData[i * 6 + 4] = i * 4 + 3;
				dsr.renderer.mesh.indexData[i * 6 + 5] = i * 4 + 0;
			}
			SDL_Log("ticks spent in SIMD: %i", SDL_GetTicks() - timer);
			glBindBuffer(GL_ARRAY_BUFFER, dsr.renderer.VAO.VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, dsr.spriteCount*sizeof(SGL_SpriteVertex) * 4, dsr.renderer.mesh.vertexData);
			glBindBuffer(GL_ARRAY_BUFFER, dsr.renderer.VAO.EBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, dsr.spriteCount*sizeof(U32) * 6, dsr.renderer.mesh.indexData);
			//SGL_SpriteVertex* sVert = (SGL_SpriteVertex*)vertexData;
			//for (size_t i = 0; i < 4; i++)
			//{
			//	SDL_Log("Vert:%i, X:%f, Y:%f, U:%f, V:%f", i, sVert[i].pos.x, sVert[i].pos.y, sVert[i].texCoord.x, sVert[i].texCoord.y);
			//}
			SGL_DrawDynamicRenderer(rContext, &dsr.renderer, dsr.spriteCount * 6);
		}
		//*/
	}
}
