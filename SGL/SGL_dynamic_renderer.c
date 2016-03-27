#include "SGL.h"

SGL_DynamicRenderer SGL_CreateDynamicRenderer(const SGL_Shader* shader, U32 texHandle, U32 vertexCount, U32 indexCount)
{
	SGL_DynamicRenderer dr;
	dr.mesh.vertexData = SDL_malloc(shader->vertexSize * vertexCount);
	dr.mesh.indexData = SDL_malloc(sizeof(U32) * indexCount);
	dr.mesh.vertexCount = vertexCount;
	dr.mesh.indexCount = indexCount;
	dr.vertexSize = shader->vertexSize;
	dr.texHandle = texHandle;
	dr.shaderHandle = shader->handle;
	dr.currentIndexCount = 0;
	dr.currentVertexCount = 0;
	shader->bindFunction(&dr.VAO, shader->handle, vertexCount, indexCount, GL_DYNAMIC_DRAW);
	return dr;
}

void SGL_DrawDynamicRenderer(SGL_RenderContext* rContext, const SGL_DynamicRenderer* dr, U32 indexCount)
{
	glBindVertexArray(dr->VAO.handle);
	SGL_BindShader(rContext, dr->shaderHandle);
	glDrawArrays(GL_TRIANGLES, 0, indexCount);
	glBindVertexArray(0);
	//glBufferData(GL_ARRAY_BUFFER, maxCapacity*sizeof(Particle), NULL, GL_STATIC_DRAW);
}
