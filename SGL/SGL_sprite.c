#include "SGL.h"
inline void LockBuffer(GLsync* gSync)
{
	if (*gSync)
	{
		glDeleteSync(*gSync);
	}
	*gSync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}
SGL_DynamicRenderer SGL_CreateSectorRenderer(const SGL_RenderContext * rContext)
{
	SGL_DynamicRenderer dsr;
	glGenVertexArrays(1, &dsr.VAO.handle);
	glGenBuffers(1, &dsr.VAO.VBO);
	//glBindBuffer(GL_ARRAY_BUFFER, dsr.VAO.VBO);
	dsr.mesh.vertexData = malloc(sizeof(SGL_ColoredSpriteVertex) * 131);
	rContext->shaders[SGL_SHADER_LIGHT].bindFunction(dsr.VAO.handle, dsr.VAO.VBO, rContext->shaders[SGL_SHADER_LIGHT].handle);
	return dsr;
}
SGL_DynamicRenderer SGL_CreatePointSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D * tex, const SGL_RenderContext * rContext)
{
	SGL_DynamicRenderer psr;
	psr.mesh.vertexCount = spriteCountMax;
	psr.texHandle = tex->handle;
	psr.shaderHandle = rContext->shaders[SGL_SHADER_POINTSPRITE].handle;
	glGenBuffers(1, &psr.VAO.VBO);
	glGenVertexArrays(1, &psr.VAO.handle);
	glBindVertexArray(psr.VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, psr.VAO.VBO);
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, rContext->shaders[SGL_SHADER_POINTSPRITE].vertexSize *  spriteCountMax, NULL, flags);
	psr.mesh.vertexData = glMapBufferRange(GL_ARRAY_BUFFER, 0, rContext->shaders[SGL_SHADER_POINTSPRITE].vertexSize * spriteCountMax, flags);
	SGL_CHECK_GL_ERROR;
	glUseProgram(psr.shaderHandle);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, rContext->shaders[SGL_SHADER_POINTSPRITE].vertexSize, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, rContext->shaders[SGL_SHADER_POINTSPRITE].vertexSize, BUFFER_OFFSET(sizeof(SGL_Vec2)));
	glBindVertexArray(0);
	SGL_CHECK_GL_ERROR;
	psr.texSize.x = (F32)tex->size.x;
	psr.texSize.y = (F32)tex->size.y;
	psr.spriteCount = 0;
	psr.spriteCountMax = spriteCountMax;
	return psr;
}
SGL_DynamicRenderer SGL_CreatePointRenderer(U32 spriteCountMax, const U32 shaderEnum, const SGL_RenderContext * rContext)
{
	SGL_DynamicRenderer pr;
	pr.mesh.vertexCount = spriteCountMax;
	pr.shaderHandle = rContext->shaders[shaderEnum].handle;
	glGenBuffers(1, &pr.VAO.VBO);
	glGenVertexArrays(1, &pr.VAO.handle);
	glBindVertexArray(pr.VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, pr.VAO.VBO);
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, rContext->shaders[shaderEnum].vertexSize *  spriteCountMax, NULL, flags);
	pr.mesh.vertexData = glMapBufferRange(GL_ARRAY_BUFFER, 0, rContext->shaders[shaderEnum].vertexSize * spriteCountMax, flags);
	rContext->shaders[shaderEnum].bindFunction(pr.VAO.handle, pr.VAO.VBO, rContext->shaders[SGL_SHADER_SPRITE].handle);
	SGL_CHECK_GL_ERROR;
	pr.spriteCount = 0;
	pr.spriteCountMax = spriteCountMax;
	return pr;
}
SGL_SpriteRenderer SGL_CreateSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D * tex, const SGL_RenderContext * rContext)
{
	SGL_SpriteRenderer ssr;
	ssr.texHandle = tex->handle;
	ssr.shaderHandle = rContext->shaders[SGL_SHADER_SPRITE].handle;
	glGenBuffers(1, &ssr.VAO.VBO);
	glGenBuffers(1, &ssr.VAO.EBO);
	glGenVertexArrays(1, &ssr.VAO.handle);
	glBindVertexArray(ssr.VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, ssr.VAO.VBO);
	glBufferData(GL_ARRAY_BUFFER, rContext->shaders[SGL_SHADER_SPRITE].vertexSize *  spriteCountMax * 4, NULL, GL_STREAM_DRAW);
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
	ssr.vertexData = glMapBufferRange(GL_ARRAY_BUFFER, 0, rContext->shaders[SGL_SHADER_SPRITE].vertexSize * spriteCountMax * 4, flags);
	SGL_CHECK_GL_ERROR;
	U32* indexData = SDL_malloc(sizeof(U32) * spriteCountMax * 5);
	for (U32 i = 0; i < spriteCountMax; i++)
	{
		indexData[i * 5 + 0] = i * 4 + 0;
		indexData[i * 5 + 1] = i * 4 + 1;
		indexData[i * 5 + 2] = i * 4 + 2;
		indexData[i * 5 + 3] = i * 4 + 3;
		indexData[i * 5 + 4] = UINT_MAX;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ssr.VAO.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(U32) * spriteCountMax * 5, indexData, GL_STATIC_DRAW);
	SDL_free(indexData);
	rContext->shaders[SGL_SHADER_SPRITE].bindFunction(ssr.VAO.handle, ssr.VAO.VBO, rContext->shaders[SGL_SHADER_SPRITE].handle);
	ssr.texSize.x = (F32)tex->size.x;
	ssr.texSize.y = (F32)tex->size.y;
	ssr.spriteCount = 0;
	ssr.spriteCountMax = spriteCountMax;
	return ssr;
}
SGL_DynamicRenderer SGL_CreateSimpleSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D * tex, const SGL_RenderContext * rContext)
{
	SGL_DynamicRenderer ssr;
	ssr.mesh.vertexCount = spriteCountMax * 4;
	ssr.mesh.indexCount = spriteCountMax * 6;
	ssr.texHandle = tex->handle;
	ssr.shaderHandle = rContext->shaders[SGL_SHADER_SPRITE].handle;
	glGenBuffers(1, &ssr.VAO.VBO);
	glGenBuffers(1, &ssr.VAO.EBO);
	glGenVertexArrays(1, &ssr.VAO.handle);
	glBindVertexArray(ssr.VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, ssr.VAO.VBO);
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
	glBufferStorage(GL_ARRAY_BUFFER, rContext->shaders[SGL_SHADER_SPRITE].vertexSize *  spriteCountMax * 4, NULL, flags);
	ssr.mesh.vertexData = glMapBufferRange(GL_ARRAY_BUFFER, 0, rContext->shaders[SGL_SHADER_SPRITE].vertexSize * spriteCountMax * 4, flags);
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
	rContext->shaders[SGL_SHADER_SPRITE].bindFunction(ssr.VAO.handle, ssr.VAO.VBO, rContext->shaders[SGL_SHADER_SPRITE].handle);
	ssr.texSize.x = (F32)tex->size.x;
	ssr.texSize.y = (F32)tex->size.y;
	ssr.spriteCount = 0;
	ssr.spriteCountMax = spriteCountMax;
	return ssr;
}
SGL_StaticRenderer SGL_CreateStaticSpriteRenderer(U32 spriteCountMax, const SGL_Tex2D * tex, const SGL_RenderContext * rContext)
{
	SGL_StaticRenderer ssr;
	ssr.mesh.vertexCount = spriteCountMax * 4;
	ssr.mesh.indexCount = spriteCountMax * 6;
	ssr.texHandle = tex->handle;
	ssr.shaderHandle = rContext->shaders[SGL_SHADER_OFFSETSPRITE].handle;
	glGenBuffers(1, &ssr.VAO.VBO);
	glGenBuffers(1, &ssr.VAO.EBO);
	glGenVertexArrays(1, &ssr.VAO.handle);
	glBindVertexArray(ssr.VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, ssr.VAO.VBO);
	ssr.mesh.vertexData = SDL_malloc(rContext->shaders[SGL_SHADER_OFFSETSPRITE].vertexSize *  spriteCountMax * 4);
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
	rContext->shaders[SGL_SHADER_OFFSETSPRITE].bindFunction(ssr.VAO.handle, ssr.VAO.VBO, rContext->shaders[SGL_SHADER_OFFSETSPRITE].handle);
	ssr.texSize.x = (F32)tex->size.x;
	ssr.texSize.y = (F32)tex->size.y;
	ssr.spriteCount = 0;
	ssr.spriteCountMax = spriteCountMax;
	return ssr;
}
SGL_StaticRenderer SGL_CreateStaticColorRenderer(U32 spriteCountMax, U32 shader, const SGL_RenderContext * rContext)
{
	SGL_StaticRenderer scr;
	scr.mesh.vertexCount = spriteCountMax * 4;
	scr.mesh.indexCount = spriteCountMax * 6;
	scr.shaderHandle = rContext->shaders[shader].handle;
	glGenBuffers(1, &scr.VAO.VBO);
	glGenBuffers(1, &scr.VAO.EBO);
	glGenVertexArrays(1, &scr.VAO.handle);
	glBindVertexArray(scr.VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, scr.VAO.VBO);
	scr.mesh.vertexData = SDL_malloc(rContext->shaders[shader].vertexSize *  spriteCountMax * 4);
	scr.mesh.indexData = SDL_malloc(sizeof(U32) * spriteCountMax * 6);
	for (U32 i = 0; i < spriteCountMax; i++)
	{
		scr.mesh.indexData[i * 6 + 0] = i * 4 + 0;
		scr.mesh.indexData[i * 6 + 1] = i * 4 + 1;
		scr.mesh.indexData[i * 6 + 2] = i * 4 + 2;
		scr.mesh.indexData[i * 6 + 3] = i * 4 + 2;
		scr.mesh.indexData[i * 6 + 4] = i * 4 + 3;
		scr.mesh.indexData[i * 6 + 5] = i * 4 + 0;
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scr.VAO.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(U32) * spriteCountMax * 6, scr.mesh.indexData, GL_STATIC_DRAW);
	SDL_free(scr.mesh.indexData);
	SGL_CHECK_GL_ERROR;
	rContext->shaders[shader].bindFunction(scr.VAO.handle, scr.VAO.VBO, rContext->shaders[shader].handle);
	scr.spriteCount = 0;
	scr.spriteCountMax = spriteCountMax;
	return scr;
}
void SGL_DrawLightSector(float lightSize, SGL_Vec2 position, SGL_Vec2 direction, F32 angle,const SGL_Tex2D* tex, SGL_DynamicRenderer * renderer, const SGL_RenderContext * rContext)
{
	const F64 dirAngle = SDL_atan2((F64)direction.y, (F64)direction.x);
	const F64 padAngle = 12.566370614272 / (F64)lightSize;
	const F32 startAngle = (F32)(dirAngle - ((F64)angle+ padAngle));
	const F32 stepAngle = 50.2654824576f/lightSize;
	F32 lastStepAngle = (angle+(F32)padAngle)*2.0f;
	lastStepAngle = lastStepAngle > (F32)(2.0*M_PI) ? (F32)(2.0*M_PI) / stepAngle : lastStepAngle / stepAngle;
	U32 steps = (U32)lastStepAngle;
	SGL_ColoredSpriteVertex* circleVertices = (SGL_ColoredSpriteVertex*)renderer->mesh.vertexData;
	//SGL_Vec2 center = { 0.0f,0.0f };
	SGL_Color color = { 0,255,255,255 };
	SGL_Color colors[] = { { 255,0,255,128 },{ 0,255,0,128 } };
	circleVertices[0].pos = position;
	circleVertices[0].uvs.x = 0.5f;
	//this is bit weird, but y axis is reversed
	circleVertices[0].uvs.y = 0.5f;
	circleVertices[0].color = color;
	const F32 halfLight = lightSize*0.5f;
	for (U32 i = 0; i <= steps; i++)
	{
		F32 angle = startAngle + stepAngle * (F32)i;
		F32 sin = SDL_sinf(angle);
		F32 cos = SDL_cosf(angle);
		circleVertices[i+1].pos.x = position.x + cos*halfLight;
		circleVertices[i+1].pos.y = position.y + sin*halfLight;
		circleVertices[i+1].uvs.x = cos*0.5f + 0.5f;
		//this is bit weird, but y axis is reversed
		circleVertices[i+1].uvs.y = sin*-0.5f + 0.5f;
		circleVertices[i+1].color = colors[i%2];
	}
	{
		F32 angle = startAngle + stepAngle * lastStepAngle;
		F32 sin = SDL_sinf(angle);
		F32 cos = SDL_cosf(angle);
		circleVertices[steps+2].pos.x = position.x + cos*halfLight;
		circleVertices[steps+2].pos.y = position.y + sin*halfLight;
		circleVertices[steps+2].uvs.x = cos*0.5f + 0.5f;
		//this is bit weird, but y axis is reversed
		circleVertices[steps+2].uvs.y = sin*-0.5f + 0.5f;
		circleVertices[steps+2].color = colors[steps%2];
	}
	glBindVertexArray(renderer->VAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, renderer->VAO.VBO);
	glBufferData(GL_ARRAY_BUFFER, (steps+3)*sizeof(SGL_ColoredSpriteVertex), circleVertices, GL_STATIC_DRAW);
	glBindTexture(GL_TEXTURE_2D, tex->handle);
	SGL_BindShader(rContext->shaders[SGL_SHADER_LIGHT].handle, rContext);
	GLint loc = glGetUniformLocation(rContext->shaders[SGL_SHADER_LIGHT].handle, "resolution");
	if (loc != -1)
	{
		glUniform2f(loc, lightSize, lightSize);
	}
	glDrawArrays(GL_TRIANGLE_FAN, 0, (steps + 3));
	SGL_CHECK_GL_ERROR;
	if (rContext->debug)
	{
		SGL_BindShader(rContext->shaders[SGL_SHADER_SECTOR].handle, rContext);
		glDrawArrays(GL_TRIANGLE_FAN, 0, (steps + 3));
	}
	SGL_CHECK_GL_ERROR;
	
}
void SGL_MapShadows(float lightSize, U32 texHandle, const SGL_RenderContext * rContext)
{
	glBindVertexArray(rContext->shadowVAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, rContext->shadowVAO.VBO);
	const SGL_Vec2 vertices[] =
	{

		{-1.0f,-1.0f },{ 0.0f,0.0f },
		{-1.0f, 1.0f },{ 0.0f,1.0f },
		{ 1.0f,-1.0f },{ 1.0f,0.0f },
		{ 1.0f, 1.0f },{ 1.0f,1.0f },
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindTexture(GL_TEXTURE_2D, texHandle);
	SGL_BindShader(rContext->shaders[SGL_SHADER_SHADOWMAP].handle, rContext);
	GLint loc = glGetUniformLocation(rContext->shaders[SGL_SHADER_SHADOWMAP].handle, "resolution");
	if (loc != -1)
	{
		glUniform2f(loc, lightSize, lightSize);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void SGL_MapSectorShadows(float lightSize, SGL_Vec2 position, SGL_Vec2 direction, F32 angle,const SGL_Tex2D* tex, const SGL_RenderContext * rContext)
{
	const F64 dirAngle = SDL_atan2((F64)direction.y, (F64)direction.x);
	F64 dx0 = dirAngle - (F64)angle;
	F64 dx1 = dirAngle + (F64)angle;

	glBindVertexArray(rContext->shadowVAO.handle);
	SGL_CHECK_GL_ERROR;
	glBindBuffer(GL_ARRAY_BUFFER, rContext->shadowVAO.VBO);
	SGL_CHECK_GL_ERROR;
	glBindTexture(GL_TEXTURE_2D, tex->handle);
	SGL_CHECK_GL_ERROR;
	SGL_BindShader(rContext->shaders[SGL_SHADER_SHADOWMAP].handle, rContext);
	SGL_CHECK_GL_ERROR;
	GLint loc = glGetUniformLocation(rContext->shaders[SGL_SHADER_SHADOWMAP].handle, "sizePosScale");
	SGL_CHECK_GL_ERROR;
	SGL_Vec4 posV4 = { position.x, position.y, 0.0f, 1.0f };
	//you actually must do this later on
	//you should also take the camera rotation and scale into consideration when multiplying the variables with 0.5f or maybe not?
	////posV4 = SM_M4V4Multiply(&rContext->cameras[rContext->boundCamera].vPMatrix, &posV4);
	SGL_Vec4 camPosV4 = SM_M4V4Multiply(&rContext->cameras[rContext->boundCamera].vPMatrix, &rContext->cameras[rContext->boundCamera].position);
	posV4.x -= camPosV4.x;
	posV4.y -= camPosV4.y;
	posV4.x *= 0.5f;
	posV4.y *= 0.5f;

	if (loc != -1)
	{
		SGL_Vec2 sizePosScale[3] =
		{
			{ lightSize, lightSize },
			//for testing purposes only
			//a->m00 * b->x + a->m01 * b->y
			//a->m10 * b->x + a->m11 * b->y

			//{ position.x * rContext->cameras[rContext->boundCamera].vPMatrix.m00 + position.y * rContext->cameras[rContext->boundCamera].vPMatrix.m01,
			//position.x * rContext->cameras[rContext->boundCamera].vPMatrix.m10 + position.y * rContext->cameras[rContext->boundCamera].vPMatrix.m11 },
			//{ posV4.x, posV4.y },
			//{ -0.375f , -0.375f },
			{ lightSize / (F32)tex->size.x, lightSize / (F32)tex->size.y}
		};
		glUniform2fv(loc, 3, sizePosScale);
		SGL_CHECK_GL_ERROR;
	}
	if (dx0 < -M_PI)
	{
		dx0 = (dx0 + M_PI*2.0)/M_PI;
		dx1 = dx1/M_PI;
		F32 x0 = (F32)-dx0;
		F32 x1 = (F32)-dx1;
		F32 u0 = (x0 + 1.0f)*0.5f;
		F32 u1 = (x1 + 1.0f)*0.5f;
		const SGL_Vec2 vertices[] =
		{

			{-1.0f,-1.0f },{ 0.0f,0.0f },
			{-1.0f, 1.0f },{ 0.0f,1.0f },
			{   x0,-1.0f },{   u0,0.0f },
			{   x0,-1.0f },{   u0,0.0f },
			{-1.0f, 1.0f },{ 0.0f,1.0f },
			{   x0, 1.0f },{   u0,1.0f },
			{   x1,-1.0f },{   u1,0.0f },
			{   x1, 1.0f },{   u1,1.0f },
			{ 1.0f,-1.0f },{ 1.0f,0.0f },
			{ 1.0f,-1.0f },{ 1.0f,0.0f },
			{   x1, 1.0f },{   u1,1.0f },
			{ 1.0f, 1.0f },{ 1.0f,1.0f },
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 12);
		SGL_CHECK_GL_ERROR;
	}
	else if (dx1 > M_PI)
	{
		dx0 = dx0 / M_PI;
		dx1 = (dx1 - M_PI*2.0) / M_PI;
		F32 x0 = (F32)-dx0;
		F32 x1 = (F32)-dx1;
		F32 u0 = (x0 + 1.0f)*0.5f;
		F32 u1 = (x1 + 1.0f)*0.5f;
		const SGL_Vec2 vertices[] =
		{

			{-1.0f,-1.0f },{ 0.0f,0.0f },
			{-1.0f, 1.0f },{ 0.0f,1.0f },
			{   x0,-1.0f },{   u0,0.0f },
			{   x0,-1.0f },{   u0,0.0f },
			{-1.0f, 1.0f },{ 0.0f,1.0f },
			{   x0, 1.0f },{   u0,1.0f },
			{   x1,-1.0f },{   u1,0.0f },
			{   x1, 1.0f },{   u1,1.0f },
			{ 1.0f,-1.0f },{ 1.0f,0.0f },
			{ 1.0f,-1.0f },{ 1.0f,0.0f },
			{   x1, 1.0f },{   u1,1.0f },
			{ 1.0f, 1.0f },{ 1.0f,1.0f },
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 12);
		SGL_CHECK_GL_ERROR;
	}
	else
	{
		F32 x0 = (F32)-(dx0/M_PI);
		F32 x1 = (F32)-(dx1/M_PI);
		F32 u0 = (x0 + 1.0f)*0.5f;
		F32 u1 = (x1 + 1.0f)*0.5f;
		const SGL_Vec2 vertices[] =
		{

			{ x0,-1.0f },{ u0,0.0f },
			{ x0, 1.0f },{ u0,1.0f },
			{ x1,-1.0f },{ u1,0.0f },
			{ x1, 1.0f },{ u1,1.0f },
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		SGL_CHECK_GL_ERROR;
	}
	
}
void SGL_DrawShadows(float lightSize, U32 texHandle, const SGL_RenderContext * rContext)
{
	const SGL_ColoredSpriteVertex vertices[] =
	{
		{{-lightSize*0.5f,-lightSize*0.5f },{255,255,255,255},{ 0.0f,1.0f }},
		{{-lightSize*0.5f, lightSize*0.5f },{255,255,255,255},{ 0.0f,0.0f }},
		{{ lightSize*0.5f,-lightSize*0.5f },{255,255,255,255},{ 1.0f,1.0f }},
		{{ lightSize*0.5f, lightSize*0.5f },{255,255,255,255},{ 1.0f,0.0f }},
	};
	glBindVertexArray(rContext->lightVAO.handle);
	glBindBuffer(GL_ARRAY_BUFFER, rContext->lightVAO.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindTexture(GL_TEXTURE_2D, texHandle);
	SGL_BindShader(rContext->shaders[SGL_SHADER_LIGHT].handle, rContext);
	GLint loc = glGetUniformLocation(rContext->shaders[SGL_SHADER_LIGHT].handle, "resolution");
	if (loc != -1)
	{
		glUniform2f(loc, lightSize, lightSize);
	}
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	SGL_CHECK_GL_ERROR;
	int x = 0;
}
void SGL_StaticSpriteRendererDraw(SGL_StaticRenderer * ssr, const SGL_RenderContext * rContext)
{
	glBindTexture(GL_TEXTURE_2D, ssr->texHandle);
	SGL_BindShader(ssr->shaderHandle, rContext);
	glBindVertexArray(ssr->VAO.handle);
	glDrawElements(GL_TRIANGLES, ssr->spriteCount * 6, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}
void SGL_StaticSpriteRendererDrawRange(SGL_StaticRenderer * ssr, const SGL_RenderContext * rContext, U32 startSprite, U32 count, SGL_Vec2 offset)
{
	glBindTexture(GL_TEXTURE_2D, ssr->texHandle);
	SGL_BindShader(ssr->shaderHandle, rContext);
	GLint loc = glGetUniformLocation(ssr->shaderHandle, "offset");
	if (loc != -1)
	{
		glUniform2f(loc, offset.x, offset.y);
	}
	glBindVertexArray(ssr->VAO.handle);
	int g = 0;
	glDrawElements(GL_TRIANGLES, count*6, GL_UNSIGNED_INT, (GLvoid*)(startSprite*6*4)); //6 vertices with 4 float values
	glBindVertexArray(0);
}
void SGL_StaticRendererDrawSP(SGL_StaticRenderer * scr, const SGL_RenderContext * rContext)
{
	SGL_BindShader(scr->shaderHandle, rContext);
	glBindVertexArray(scr->VAO.handle);
	glDrawElements(GL_TRIANGLES, scr->spriteCount * 6, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	SGL_CHECK_GL_ERROR;
}
void SGL_SpriteRendererDraw(SGL_SpriteRenderer* ssr, const SGL_RenderContext * rContext)
{
	glBindBuffer(GL_ARRAY_BUFFER, ssr->VAO.VBO);
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindTexture(GL_TEXTURE_2D, ssr->texHandle);
	SGL_BindShader(ssr->shaderHandle, rContext);
	glBindVertexArray(ssr->VAO.handle);
	glDrawElements(GL_TRIANGLE_STRIP, ssr->spriteCount * 6, GL_UNSIGNED_INT, NULL);
	//Orphan the buffer
	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
	ssr->vertexData = glMapBufferRange(GL_ARRAY_BUFFER, 0, rContext->shaders[SGL_SHADER_SPRITE].vertexSize * ssr->spriteCountMax * 4, flags);
	glBindVertexArray(0);
	ssr->spriteCount = 0;
}
void SGL_SimpleSpriteRendererDraw(SGL_DynamicRenderer* ssr, const SGL_RenderContext * rContext)
{
	glBindTexture(GL_TEXTURE_2D, ssr->texHandle);
	SGL_BindShader(ssr->shaderHandle, rContext);
	glBindVertexArray(ssr->VAO.handle);
	glDrawElementsBaseVertex(GL_TRIANGLES, ssr->spriteCount * 6, GL_UNSIGNED_INT, NULL, ssr->mesh.vertexCount);
	glBindVertexArray(0);
	ssr->spriteCount = 0;
}
void SGL_PointSpriteRendererDraw(SGL_DynamicRenderer * renderer, const SGL_RenderContext * rContext)
{
	glBindTexture(GL_TEXTURE_2D, renderer->texHandle);
	SGL_BindShader(renderer->shaderHandle, rContext);
	GLint loc = glGetUniformLocation(renderer->shaderHandle, "texSize");
	if (loc != -1)
	{
		glUniform2f(loc, renderer->texSize.x, renderer->texSize.y);
	}
	glBindVertexArray(renderer->VAO.handle);
	glDrawArrays(GL_POINTS, renderer->spriteCountMax, renderer->spriteCount);
	glBindVertexArray(0);
	renderer->spriteCount = 0;
}
void SGL_PointRendererDraw(SGL_DynamicRenderer * renderer, const SGL_RenderContext * rContext)
{
	SGL_BindShader(renderer->shaderHandle, rContext);
	glBindVertexArray(renderer->VAO.handle);
	glDrawArrays(GL_POINTS, renderer->spriteCountMax, renderer->spriteCount);
	glBindVertexArray(0);
	renderer->spriteCount = 0;
}
void SGL_DestroySSRenderer(SGL_DynamicRenderer * ssr)
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
