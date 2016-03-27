#include "SGL.h"
#include <GL\glew.h>
const SGL_ShaderInfo colorShader[] =
{
	{
		GL_VERTEX_SHADER,
		"#version 330 core\n"
		"layout (std140) uniform globalMatrices\n"
		"{\n"
		"	mat4 vPMatrix;\n"
		"};\n"
		"layout(location = 0) in vec2 vPosition;"
		"layout(location = 1) in vec4 vColor;"
		"out vec4 color;"
		"void main()"
		"{"
		"	color = vColor;"
		"	gl_Position = vPMatrix * vec4(vPosition, 1.0, 1.0);"
		"}"
	},
	{
		GL_FRAGMENT_SHADER,
		"#version 330 core\n"
		"in vec4 color;"
		"out vec4 fColor;"
		"void main()"
		"{"
		"	fColor = color;"
		"}"
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo spriteShader[] =
{
	{
		GL_VERTEX_SHADER,
		"#version 330\n"
		"layout (std140) uniform globalMatrices\n"
		"{\n"
		"	mat4 vPMatrix;\n"
		"};\n"
		"layout(location = 0) in vec2 vPosition;"	//0
		"layout(location = 1) in vec2 vTexCoord;"	//2
		"out vec2 texCoord;"
		"void main()"
		"{"
		"	texCoord = vTexCoord;"
		"	gl_Position = vPMatrix * vec4(vPosition, 1.0, 1.0);"
		"}"
	},
	{
		GL_FRAGMENT_SHADER,
		"#version 330 core\n"
		"in vec2 texCoord;"
		"out vec4 fColor;"
		"uniform sampler2D tex;"
		"void main()"
		"{"
		"	fColor = texture(tex, texCoord);"
		"}"
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo coloredSpriteShader[] =
{
	{
		GL_VERTEX_SHADER,
		"#version 330\n"
		"layout (std140) uniform globalMatrices\n"
		"{\n"
		"	mat4 vPMatrix;\n"
		"};\n"
		"layout(location = 0) in vec2 vPosition;"	//0
		"layout(location = 1) in vec4 vColor;"		//1
		"layout(location = 2) in vec2 vTexCoord;"	//2
		"out vec4 color;"
		"out vec2 texCoord;"
		"void main()"
		"{"
		"	color = vColor;"
		"	texCoord = vTexCoord;"
		"	gl_Position = vPMatrix * vec4(vPosition, 1.0, 1.0);"
		"}"
	},
	{
		GL_FRAGMENT_SHADER,
		"#version 330 core\n"
		"in vec4 color;"
		"in vec2 texCoord;"
		"out vec4 fColor;"
		"uniform sampler2D tex;"
		"void main()"
		"{"
		"	fColor = texture(tex, texCoord) * color;"
		"}"
	},
	{
		GL_NONE,
		NULL
	}
};
const char* shaderNames[] =
{
	"         Color Shader",
	"        Sprite Shader",
	"Colored Sprite Shader",
};
const SGL_ShaderInfo* builtInShaders[] =
{
	&colorShader,
	&spriteShader,
	&coloredSpriteShader,
};
const U32 shaderVertSizes[] =
{
	sizeof(SGL_Vec2) + sizeof(SGL_Color),
	sizeof(SGL_Vec2) + sizeof(SGL_Vec2),
	sizeof(SGL_Vec2) + sizeof(SGL_Vec2) + sizeof(SGL_Color),
};
void colorShaderBindFunction(void* vao, U32 shaderHandle, U32 vertexCount, U32 indexCount, U32 drawType)
{
	SGL_VAO* VAO = (SGL_VAO*)vao;
	glGenBuffers(1, &VAO->VBO);

	glGenVertexArrays(1, &VAO->handle);
	glBindVertexArray(VAO->handle);

	glBindBuffer(GL_ARRAY_BUFFER, VAO->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * shaderVertSizes[SGL_SHADER_COLOR], NULL, drawType);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VAO->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), NULL, drawType);

	//binding starts here
	glUseProgram(shaderHandle);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, shaderVertSizes[SGL_SHADER_COLOR], BUFFER_OFFSET(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, shaderVertSizes[SGL_SHADER_COLOR], BUFFER_OFFSET(sizeof(SGL_Vec2)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void spriteShaderBindFunction(void* vao, U32 shaderHandle, U32 vertexCount, U32 indexCount, U32 drawType)
{
	SGL_VAO* VAO = (SGL_VAO*)vao;
	glGenBuffers(1, &VAO->VBO);

	glGenVertexArrays(1, &VAO->handle);
	glBindVertexArray(VAO->handle);

	glBindBuffer(GL_ARRAY_BUFFER, VAO->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * shaderVertSizes[SGL_SHADER_SPRITE], NULL, drawType);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VAO->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), NULL, drawType);

	glUseProgram(shaderHandle);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, shaderVertSizes[SGL_SHADER_SPRITE], BUFFER_OFFSET(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, shaderVertSizes[SGL_SHADER_SPRITE], BUFFER_OFFSET(sizeof(SGL_Vec2)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void coloredSpriteShaderBindFunction(void* vao, U32 shaderHandle, U32 vertexCount, U32 indexCount, U32 drawType)
{
	SGL_VAO* VAO = (SGL_VAO*)vao;
	glGenBuffers(1, &VAO->VBO);

	glGenVertexArrays(1, &VAO->handle);
	glBindVertexArray(VAO->handle);

	glBindBuffer(GL_ARRAY_BUFFER, VAO->VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * shaderVertSizes[SGL_SHADER_COLOREDSPRITE], NULL, drawType);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VAO->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(U32), NULL, drawType);

	glUseProgram(shaderHandle);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, shaderVertSizes[SGL_SHADER_COLOREDSPRITE], BUFFER_OFFSET(0));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, shaderVertSizes[SGL_SHADER_COLOREDSPRITE], BUFFER_OFFSET(sizeof(SGL_Vec2)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, shaderVertSizes[SGL_SHADER_COLOREDSPRITE], BUFFER_OFFSET(sizeof(SGL_Vec2)+sizeof(SGL_Color)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
const void(*shaderFunctions[])(void* vao, U32 shaderHandle, U32 vertexCount, U32 indexCount, U32 drawType) =
{
	&colorShaderBindFunction,
	&spriteShaderBindFunction,
	&coloredSpriteShaderBindFunction,
};
inline U32 CreateProgram(SGL_ShaderInfo* shaders)
{
	if (shaders == NULL)
	{
		SDL_Log("Shader loading failed, ShaderInfo is null.");
		return 0;
	}
	GLuint program = glCreateProgram();
	//there is currently only 4 shader types but making space for 8 handles just to be sure
	GLuint shaderHandles[8] = {0};
	for (size_t i = 0; shaders[i].type != GL_NONE; i++)
	{
		shaderHandles[i] = glCreateShader(shaders[i].type);
		const GLchar* source = shaders[i].source;
		if (source == NULL)
		{
			for (size_t j = 0; j < i; j++)
			{
				glDeleteShader(shaderHandles[i]);
			}
			return 0;
		}
		glShaderSource(shaderHandles[i], 1, &source, NULL);
		glCompileShader(shaderHandles[i]);
		GLint compiled;
		glGetShaderiv(shaderHandles[i], GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
#if !defined(NDEBUG)
			GLsizei len;
			glGetShaderiv(shaderHandles[i], GL_INFO_LOG_LENGTH, &len);

			GLchar* log = SDL_malloc(sizeof(GLchar)*(len + 1));
			glGetShaderInfoLog(shaderHandles[i], len, &len, log);
			//this takes care of deletion
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Shader Compilation Failed", log, NULL);
			SDL_free(log);
#endif /* DEBUG */
			return 0;
		}
		glAttachShader(program, shaderHandles[i]);
	}
	glLinkProgram(program);
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
#if !defined(NDEBUG)
		GLsizei len;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

		GLchar* log = SDL_malloc(sizeof(GLchar)*(len + 1));
		glGetProgramInfoLog(program, len, &len, log);
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Shader linking failed", log, NULL);
		SDL_free(log);
#endif /* DEBUG */
		for (size_t i = 0; shaderHandles[i] != 0; i++)
		{
			glDeleteShader(shaderHandles[i]);
		}
		return 0;
	}
	return program;
}
inline void BindShaderToMatrixBlock(U32 shaderHandle)
{
	GLuint globalUniformBlockIndex = glGetUniformBlockIndex(shaderHandle, "globalMatrices");
	glUniformBlockBinding(shaderHandle, globalUniformBlockIndex, 0);
}
inline void BindUniformMatrixBlock(SGL_RenderContext * rContext)
{
	glGenBuffers(1, &rContext->uniformMatrixHandle);
	glBindBuffer(GL_UNIFORM_BUFFER, rContext->uniformMatrixHandle);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SGL_Mat4), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, rContext->uniformMatrixHandle);
}
U32 SGL_LoadShaders(SGL_RenderContext * rContext)
{
	SDL_Log("Loading %i Built-in Shaders:\n", SGL_SHADER_COUNT);
	U32 result = SGL_TRUE;
	for (size_t i = 0; i < SGL_SHADER_COUNT; i++)
	{
		const char* success = "Success";
		rContext->shaders[i].handle = CreateProgram(builtInShaders[i]);
		rContext->shaders[i].vertexSize = shaderVertSizes[i];
		rContext->shaders[i].bindFunction = shaderFunctions[i];
		rContext->shaders[i].name = shaderNames[i];
		BindShaderToMatrixBlock(rContext->shaders[i].handle);
		if (rContext->shaders[i].handle == 0)
		{
			result = SGL_FALSE;
			success = "Failure";
		}
		SDL_Log("%s: Handle:%i, VertexSize:%i, BindFunction:%p, Load:%s\n", rContext->shaders[i].name, rContext->shaders[i].handle, rContext->shaders[i].vertexSize, rContext->shaders[i].bindFunction, success);
	}
	BindUniformMatrixBlock(rContext);
	SDL_Log("----------------------------------------------------------------\n");
	return SGL_TRUE;
}
