#include "SGL.h"

SGL_DynamicRenderer SGL_CreateDynamicRenderer(const SGL_Shader* shader, U32 texHandle, U32 vertexCount, U32 indexCount)
{
	SGL_DynamicRenderer dr;
	dr.mesh.vertexData = SDL_malloc(shader->vertexSize * vertexCount);
	dr.mesh.indexData = SDL_malloc(sizeof(U32) * indexCount);
	dr.mesh.vertexCount = vertexCount;
	dr.mesh.indexCount = indexCount;
	dr.texHandle = texHandle;
	dr.shaderHandle = shader->handle;
	shader->bindFunction(&dr.VAO, shader->handle, vertexCount, indexCount, GL_DYNAMIC_DRAW);
	return dr;
}

void SGL_DrawDynamicRenderer(SGL_RenderContext* rContext, const SGL_DynamicRenderer* dr, U32 indexCount)
{
	glBindVertexArray(dr->VAO.handle);
	SGL_BindShader(rContext, dr->shaderHandle);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
void SGL_DrawDebugDynamicRenderer(SGL_RenderContext* rContext, const SGL_DynamicRenderer* dr, U32 indexCount)
{
	glBindVertexArray(dr->VAO.handle);
	SGL_BindShader(rContext, rContext->shaders[SGL_SHADER_DEBUG].handle);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
