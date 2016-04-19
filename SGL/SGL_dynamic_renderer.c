#include "SGL.h"

//SGL_DynamicRenderer SGL_CreateDynamicRenderer(const SGL_Shader* shader, U32 vboCount, U32 texHandle, U32 vertexCount, U32 indexCount)
//{
//	SGL_DynamicRenderer dr;
//	//dr.mesh.vertexData[0] = SDL_malloc(shader->vertexSize * vertexCount * vboCount);
//	//dr.mesh.indexData[0] = SDL_malloc(sizeof(U32) * indexCount * vboCount);
//	dr.mesh.vertexCount = vertexCount;
//	dr.mesh.indexCount = indexCount;
//	dr.texHandle = texHandle;
//	dr.shaderHandle = shader->handle;
//	dr.VAO.maxOffset = vboCount;
//	dr.VAO.bufferOffset = 0;
//	//SGL_CHECK_GL_ERROR;
//	glGenBuffers(1, &dr.VAO.VBO);
//	glGenBuffers(1, &dr.VAO.EBO);
//	glGenVertexArrays(1, &dr.VAO.handle);
//	glBindVertexArray(dr.VAO.handle);
//	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
//	glBindBuffer(GL_ARRAY_BUFFER, dr.VAO.VBO);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dr.VAO.EBO);
//	glBufferStorage(GL_ARRAY_BUFFER, shader->vertexSize * vertexCount * vboCount, NULL, flags);
//	//glBufferData(GL_ARRAY_BUFFER, shader->vertexSize * vertexCount * vboCount, NULL, GL_STREAM_DRAW);
//	SGL_CHECK_GL_ERROR;
//	glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, sizeof(U32) * indexCount * vboCount, NULL, flags);
//	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(U32) * indexCount * vboCount, NULL, GL_STREAM_DRAW);
//	SGL_CHECK_GL_ERROR;
//	dr.mesh.vertexData = glMapBufferRange(GL_ARRAY_BUFFER, 0, shader->vertexSize * vertexCount * vboCount, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
//	SGL_CHECK_GL_ERROR;
//	dr.mesh.indexData = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(U32) * indexCount * vboCount,  GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
//	SGL_CHECK_GL_ERROR;
//	//for (size_t i = 0; i < vboCount; i++)
//	//{
//	//	glBindBuffer(GL_ARRAY_BUFFER, dr.VAO.VBO[i]);
//	//	glBufferData(GL_ARRAY_BUFFER, vertexCount * shader->vertexSize, NULL, GL_DYNAMIC_DRAW);
//	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, dr.VAO.EBO[i]);
//	//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), NULL, GL_DYNAMIC_DRAW);
//	//}
//	shader->bindFunction(&dr.VAO, shader->handle);
//	//SGL_CHECK_GL_ERROR;
//	return dr;
//}
//
//void SGL_DrawDynamicRenderer(SGL_RenderContext* rContext, const SGL_DynamicRenderer* dr, U32 vertexSize, U32 indexCount)
//{
//	glBindVertexArray(dr->VAO.handle);
//	SGL_BindShader(rContext, dr->shaderHandle);
//	glDrawElementsBaseVertex(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, NULL, dr->VAO.bufferOffset*dr->mesh.vertexCount);
//	glBindVertexArray(0);
//}
//void SGL_DrawDebugDynamicRenderer(SGL_RenderContext* rContext, const SGL_DynamicRenderer* dr, U32 indexCount)
//{
//	glBindVertexArray(dr->VAO.handle);
//	SGL_BindShader(rContext, rContext->shaders[SGL_SHADER_DEBUG].handle);
//	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
//	glBindVertexArray(0);
//}
