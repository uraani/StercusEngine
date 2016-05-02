#include "SGL.h"
inline void LockBuffer(GLsync* gSync)
{
	if (*gSync)
	{
		glDeleteSync(*gSync);
	}
	*gSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}
SGL_PointSpriteRenderer SGL_CreatePointSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D * tex, const SGL_RenderContext * rContext)
{
	SGL_PointSpriteRenderer psr;
	psr.vertexCount = spriteCountMax;
	psr.texHandle = tex->handle;
	psr.shaderHandle = rContext->shaders[SGL_SHADER_POINTSPRITE].handle;
	psr.maxOffset = rContext->bufferCount;
	psr.bufferOffset = 0;
	glGenBuffers(1, &psr.VAO.VBO);
	glGenVertexArrays(1, &psr.VAO.handle);
	glBindVertexArray(psr.VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, psr.VAO.VBO);
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, rContext->shaders[SGL_SHADER_POINTSPRITE].vertexSize *  spriteCountMax * psr.maxOffset, NULL, flags);
	psr.vertexData = glMapBufferRange(GL_ARRAY_BUFFER, 0, rContext->shaders[SGL_SHADER_POINTSPRITE].vertexSize * spriteCountMax * psr.maxOffset, flags);
	SGL_CHECK_GL_ERROR;
	glUseProgram(psr.shaderHandle);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, rContext->shaders[SGL_SHADER_POINTSPRITE].vertexSize, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, rContext->shaders[SGL_SHADER_POINTSPRITE].vertexSize, BUFFER_OFFSET(sizeof(SGL_Vec2)));
	glBindVertexArray(0);
	SGL_CHECK_GL_ERROR;
	psr.texSize.x = (F32)tex->width;
	psr.texSize.y = (F32)tex->height;
	for (size_t i = 0; i < psr.maxOffset; i++)
	{
		psr.syncs[i] = NULL;
		psr.spriteCount[i] = 0;
		LockBuffer(&psr.syncs[i]);
	}
	psr.spriteCountMax = spriteCountMax;
	return psr;
}
SGL_SimpleSpriteRenderer SGL_CreateSimpleSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D * tex, const SGL_RenderContext * rContext)
{
	SGL_SimpleSpriteRenderer ssr;
	ssr.mesh.vertexCount = spriteCountMax * 4;
	ssr.mesh.indexCount = spriteCountMax * 6;
	ssr.texHandle = tex->handle;
	ssr.shaderHandle = rContext->shaders[SGL_SHADER_SPRITE].handle;
	ssr.maxOffset = rContext->bufferCount;
	ssr.bufferOffset = 0;
	glGenBuffers(1, &ssr.VAO.VBO);
	glGenBuffers(1, &ssr.VAO.EBO);
	glGenVertexArrays(1, &ssr.VAO.handle);
	glBindVertexArray(ssr.VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, ssr.VAO.VBO);
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, rContext->shaders[SGL_SHADER_SPRITE].vertexSize *  spriteCountMax * 4 * ssr.maxOffset, NULL, flags);
	ssr.mesh.vertexData = glMapBufferRange(GL_ARRAY_BUFFER, 0, rContext->shaders[SGL_SHADER_SPRITE].vertexSize * spriteCountMax * 4 * ssr.maxOffset, flags);
	SGL_CHECK_GL_ERROR;
	ssr.mesh.indexData = SDL_malloc(sizeof(U32) * spriteCountMax * 6);
	for (U32 i = 0; i < spriteCountMax; i++)
	{
		ssr.mesh.indexData[i * 6 + 0] = i * 4 + 0;
		ssr.mesh.indexData[i * 6 + 1] = i * 4 + 1;
		ssr.mesh.indexData[i * 6 + 2] = i * 4 + 2;
		ssr.mesh.indexData[i * 6 + 3] = i * 4 + 2;
		ssr.mesh.indexData[i * 6 + 4] = i * 4 + 3;
		ssr.mesh.indexData[i * 6 + 5] = i * 4 + 0;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ssr.VAO.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(U32) * spriteCountMax * 6, ssr.mesh.indexData, GL_STATIC_DRAW);
	SDL_free(ssr.mesh.indexData);
	SGL_CHECK_GL_ERROR;
	rContext->shaders[SGL_SHADER_SPRITE].bindFunction(&ssr.VAO, rContext->shaders[SGL_SHADER_SPRITE].handle);
	ssr.texSize.x = (F32)tex->width;
	ssr.texSize.y = (F32)tex->height;
	for (size_t i = 0; i < ssr.maxOffset; i++)
	{
		ssr.syncs[i] = NULL;
		ssr.spriteCount[i] = 0;
		LockBuffer(&ssr.syncs[i]);
	}
	ssr.spriteCountMax = spriteCountMax;
	return ssr;
}
SGL_StaticSpriteRenderer SGL_CreateStaticSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D * tex, const SGL_RenderContext * rContext)
{
	SGL_StaticSpriteRenderer ssr;
	ssr.mesh.vertexCount = spriteCountMax * 4;
	ssr.mesh.indexCount = spriteCountMax * 6;
	ssr.texHandle = tex->handle;
	ssr.shaderHandle = rContext->shaders[SGL_SHADER_SPRITE].handle;
	glGenBuffers(1, &ssr.VAO.VBO);
	glGenBuffers(1, &ssr.VAO.EBO);
	glGenVertexArrays(1, &ssr.VAO.handle);
	glBindVertexArray(ssr.VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, ssr.VAO.VBO);
	ssr.mesh.vertexData = SDL_malloc(rContext->shaders[SGL_SHADER_SPRITE].vertexSize *  spriteCountMax * 4);
	ssr.mesh.indexData = SDL_malloc(sizeof(U32) * spriteCountMax * 6);
	for (U32 i = 0; i < spriteCountMax; i++)
	{
		ssr.mesh.indexData[i * 6 + 0] = i * 4 + 0;
		ssr.mesh.indexData[i * 6 + 1] = i * 4 + 1;
		ssr.mesh.indexData[i * 6 + 2] = i * 4 + 2;
		ssr.mesh.indexData[i * 6 + 3] = i * 4 + 2;
		ssr.mesh.indexData[i * 6 + 4] = i * 4 + 3;
		ssr.mesh.indexData[i * 6 + 5] = i * 4 + 0;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ssr.VAO.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(U32) * spriteCountMax * 6, ssr.mesh.indexData, GL_STATIC_DRAW);
	SDL_free(ssr.mesh.indexData);
	SGL_CHECK_GL_ERROR;
	rContext->shaders[SGL_SHADER_SPRITE].bindFunction(&ssr.VAO, rContext->shaders[SGL_SHADER_SPRITE].handle);
	ssr.texSize.x = (F32)tex->width;
	ssr.texSize.y = (F32)tex->height;
	ssr.spriteCount = 0;
	ssr.spriteCountMax = spriteCountMax;
	return ssr;
}
void SGL_StaticSpriteRendererDraw(SGL_StaticSpriteRenderer * ssr, const SGL_RenderContext * rContext, float opacity)
{
	glBindTexture(GL_TEXTURE_2D, ssr->texHandle);
	SGL_BindShader(rContext, ssr->shaderHandle);
	GLint loc = glGetUniformLocation(ssr->shaderHandle, "gamma");
	if (loc != -1)
	{
		glUniform1f(loc, opacity);
	}
	glBindVertexArray(ssr->VAO.handle);
	glDrawElements(GL_TRIANGLES, ssr->spriteCount * 6, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}
void SGL_SimpleSpriteRendererDraw(SGL_SimpleSpriteRenderer* ssr, const SGL_RenderContext * rContext)
{
	glBindTexture(GL_TEXTURE_2D, ssr->texHandle);
	SGL_BindShader(rContext, ssr->shaderHandle);
	glBindVertexArray(ssr->VAO.handle);
	glDrawElementsBaseVertex(GL_TRIANGLES, ssr->spriteCount[ssr->bufferOffset] * 6, GL_UNSIGNED_INT, NULL, ssr->bufferOffset*ssr->mesh.vertexCount);
	glBindVertexArray(0);
	LockBuffer(&ssr->syncs[ssr->bufferOffset]);
	ssr->spriteCount[ssr->bufferOffset] = 0;
	ssr->bufferOffset = (ssr->bufferOffset +1) % ssr->maxOffset;
}
void SGL_PointSpriteRendererDraw(SGL_PointSpriteRenderer * renderer, const SGL_RenderContext * rContext)
{
	glBindTexture(GL_TEXTURE_2D, renderer->texHandle);
	SGL_BindShader(rContext, renderer->shaderHandle);
	GLint loc = glGetUniformLocation(renderer->shaderHandle, "texSize");
	if (loc != -1)
	{
		glUniform2f(loc, renderer->texSize.x, renderer->texSize.y);
	}
	glBindVertexArray(renderer->VAO.handle);
	glDrawArrays(GL_POINTS, renderer->spriteCountMax*renderer->bufferOffset, renderer->spriteCount[renderer->bufferOffset]);
	glBindVertexArray(0);
	LockBuffer(&renderer->syncs[renderer->bufferOffset]);
	renderer->spriteCount[renderer->bufferOffset] = 0;
	renderer->bufferOffset = (renderer->bufferOffset + 1) % renderer->maxOffset;
}
void SGL_DestroySSRenderer(SGL_SimpleSpriteRenderer * ssr)
{
	glFinish();
	glBindBuffer(GL_ARRAY_BUFFER, ssr->VAO.VBO);
	SGL_CHECK_GL_ERROR;
	glUnmapBuffer(GL_ARRAY_BUFFER);
	SGL_CHECK_GL_ERROR;
	glDeleteBuffers(1, &ssr->VAO.VBO);
	SGL_CHECK_GL_ERROR;
	glDeleteBuffers(1, &ssr->VAO.EBO);
	SGL_CHECK_GL_ERROR;
	glDeleteVertexArrays(1, &ssr->VAO.handle);
	SGL_CHECK_GL_ERROR;
}
