#include "SGL.h"
#include <GL\glew.h>
#define GLSL(src) "#version 330 core\n" #src
const SGL_ShaderInfo debugShader[] =
{
	{
		GL_VERTEX_SHADER,
		GLSL
		(
			layout (std140) uniform globalMatrices\n
			{\n
				mat4 vPMatrix;\n
			};\n
			layout(location = 0) in vec2 vPosition;
			void main()
			{
				gl_Position = vPMatrix * vec4(vPosition, 0.0, 1.0);
			}
		)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
		(
			out vec4 fColor;
			void main()
			{
				fColor = vec4(0.5,0.0,0.5,0.5);
			}
		)
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo triangleDebugShader[] =
{
	{
		GL_VERTEX_SHADER,
		GLSL
	(
		layout(location = 0) in vec2 vPosition;
		void main()
		{
			gl_Position = vec4(vPosition, 0.0, 1.0);
		}
	)
	},
	{
		GL_GEOMETRY_SHADER,
		GLSL
	(
		layout(std140) uniform globalMatrices\n
		{ \n
			mat4 vPMatrix; \n
		}; \n
		layout(triangles) in;
		layout(line_strip, max_vertices = 4) out;
		void main()
		{
			gl_Position = vPMatrix * (gl_in[0].gl_Position);
			EmitVertex();
			gl_Position = vPMatrix * (gl_in[1].gl_Position);
			EmitVertex();
			gl_Position = vPMatrix * (gl_in[2].gl_Position);
			EmitVertex();
			gl_Position = vPMatrix * (gl_in[0].gl_Position);
			EmitVertex();
			EndPrimitive();
		}
	)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
	(
		out vec4 fColor;
		void main()
		{
			fColor = vec4(1.0,0.0,0.0,1.0);
		}
	)
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo colorShader[] =
{
	{
		GL_VERTEX_SHADER,
		GLSL
		(
			layout (std140) uniform globalMatrices\n
			{\n
				mat4 vPMatrix;\n
			};\n
			layout(location = 0) in vec2 vPosition;
			layout(location = 1) in vec4 vColor;
			out vec4 color;
			void main()
			{
				color = vColor;
				gl_Position = vPMatrix * vec4(vPosition, 0.0, 1.0);
			}
		)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
		(
			in vec4 color;
			out vec4 fColor;
			void main()
			{
				fColor = color;
			}
		)
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
		GLSL
		(
			layout (std140) uniform globalMatrices\n
			{\n
				mat4 vPMatrix;\n
			};\n
			layout(location = 0) in vec2 vPosition;
			layout(location = 2) in vec2 vTexCoord;	
			out vec2 texCoord;
			void main()
			{
				texCoord = vTexCoord;
				gl_Position = vPMatrix * vec4(vPosition, 0.0, 1.0);
			}
		)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
		(
			uniform sampler2D tex;
			in vec2 texCoord;
			out vec4 fColor;
			void main()
			{
				fColor = texture(tex, texCoord);
			}
		)
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo screenSpaceSpriteShader[] =
{
	{
		GL_VERTEX_SHADER,
		GLSL
	(
		layout(location = 0) in vec2 vPosition;
		layout(location = 2) in vec2 vTexCoord;
		out vec2 texCoord;
		void main()
		{
			texCoord = vTexCoord;
			gl_Position = vec4(vPosition, 0.0, 1.0);
		}
	)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
	(
		uniform sampler2D tex;
		in vec2 texCoord;
		out vec4 fColor;
		void main()
		{
			fColor = texture(tex, texCoord);
		}
	)
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo offsetSpriteShader[] =
{
	{
		GL_VERTEX_SHADER,
		GLSL
		(
			layout (std140) uniform globalMatrices\n
			{\n
				mat4 vPMatrix;\n
			};\n
			uniform vec2 offset;
			layout(location = 0) in vec2 vPosition;
			layout(location = 2) in vec2 vTexCoord;
			out vec2 texCoord;
			void main()
			{
				texCoord = vTexCoord;
				gl_Position = vPMatrix * vec4(vPosition+offset, 0.0, 1.0);
			}
		)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
		(
			uniform sampler2D tex;
			in vec2 texCoord;
			out vec4 fColor;
			void main()
			{
				fColor = texture(tex, texCoord);
			}
		)
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo pointSpriteShader[] =
{
	{
		GL_VERTEX_SHADER,
		GLSL
		(
			layout (std140) uniform globalMatrices\n
			{\n
				mat4 vPMatrix;\n
			};\n
			layout(location = 0) in vec2 vPosition;
			layout(location = 2) in vec3 vTexCoord;
			out vec3 texCoord;
			void main()
			{
				texCoord = vTexCoord;
				gl_PointSize = vTexCoord.z;
				gl_Position = vPMatrix * vec4(vPosition, 0.0, 1.0);
			}
		)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
		(
			uniform sampler2D tex;
			uniform vec2 texSize;
			in vec3 texCoord;
			out vec4 fColor;
			void main()
			{
				fColor = texture(tex, texCoord.xy + gl_PointCoord/(texSize/texCoord.z));
			}
		)
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
		GLSL
		(
			layout (std140) uniform globalMatrices\n
			{\n
				mat4 vPMatrix;\n
			};\n
			layout(location = 0) in vec2 vPosition;
			layout(location = 1) in vec4 vColor;
			layout(location = 2) in vec2 vTexCoord;
			out vec4 color;
			out vec2 texCoord;
			void main()
			{
				color = vColor;
				texCoord = vTexCoord;
				gl_Position = vPMatrix * vec4(vPosition, 0.0, 1.0);
			}
		)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
		(
			in vec4 color;
			in vec2 texCoord;
			out vec4 fColor;
			uniform sampler2D tex;
			void main()
			{
				fColor = texture(tex, texCoord) * color;
			}
		)
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo screenSpaceColorShader[] =
{
	{
		GL_VERTEX_SHADER,
		GLSL
		(
			layout(location = 0) in vec2 vPosition;
			layout(location = 1) in vec4 vColor;
			out vec4 color;
			void main()
			{
				color = vColor;
				gl_Position = vec4(vPosition, 0.0, 1.0);
			}
		)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
		(
			in vec4 color;
			out vec4 fColor;
			void main()
			{
				fColor = color;
			}
		)
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo shadowMapShader[] =
{
	{
		GL_VERTEX_SHADER,
		GLSL
		(
			layout(location = 0) in vec2 vPosition;
			layout(location = 2) in vec2 vTexCoord;
			out vec2 texCoord;
			void main()
			{
				texCoord = vTexCoord;
				gl_Position = vec4(vPosition, 0.0, 1.0);
			}
		)
	},
	{
		GL_FRAGMENT_SHADER,
		GLSL
		(
			#define PI 3.14159265358979\n
			in vec2 texCoord;
			uniform sampler2D u_texture;
			uniform vec2 sizePosUpscale[3];
			//uniform vec4 lightPosScale;
			const float THRESHOLD = 0.75;
			//const vec2 scale = vec2(0.266666, 0.4776119);
			out vec4 fColor;
			void main()
			{
				float distance = 1.0;
				for (float y = 0.0; y<sizePosUpscale[0].y; y += 1.0)
				{
					vec2 norm = vec2(texCoord.s, y / sizePosUpscale[0].y) * 2.0 - 1.0;
					float theta = PI*1.5 + norm.x * PI;
					float r = (1.0 + norm.y) * 0.5;
					vec2 coord = (vec2(-r * sin(theta), -r * cos(theta)) * 0.5)* sizePosUpscale[2] + sizePosUpscale[1]; //remove the 0.5 and add it to sizePosScale[1] before sending it to shader??
					vec4 data = texture2D(u_texture, coord);
					float dst = y / sizePosUpscale[0].y;
					float caster = data.a;
					if (caster > THRESHOLD)
					{
						distance = dst;
						break;
					}
				}
				fColor = vec4(vec3(distance), 1.0);
			}
		)
	},
	{
		GL_NONE,
		NULL
	}
};
const SGL_ShaderInfo lightShader[] =
{
	{
		GL_VERTEX_SHADER,
		GLSL
		(
			layout (std140) uniform globalMatrices\n
			{\n
				mat4 vPMatrix;\n
			};\n
			layout(location = 0) in vec2 vPosition;
			layout(location = 1) in vec4 vColor;
			layout(location = 2) in vec2 vTexCoord;
			out vec4 color;
			out vec2 texCoord;
			void main()
			{
				color = vColor;
				texCoord = vTexCoord;
				gl_Position = vPMatrix * vec4(vPosition, 0.0, 1.0);
			}
		)
	},

	{
		GL_FRAGMENT_SHADER,
		GLSL
		(
			#define PI 3.1415926535897\n
			in vec2 texCoord;
			in vec4 color;
			uniform sampler2D u_texture;
			uniform vec2 resolution;
			uniform vec3 falloff;
			float sample(vec2 coord, float r)
			{
				return step(r, texture2D(u_texture, coord).r);
			}
			float attenuation(float x)
			{
				//return pow(4.0f*x*(1.0f - x), k);
				return 1.0 / (falloff.x + (falloff.y*x) + (falloff.z*x*x));
			}
			out vec4 fColor;
			void main()
			{
				vec2 norm = texCoord.st * 2.0 - 1.0;
				float theta = atan(norm.y, norm.x);
				float r = length(norm);
				float coord = (theta + PI) / (2.0*PI);
				vec2 tc = vec2(coord, 0.0);
				float center = sample(tc, r);
				//soft shadows
				float blur = (1.0 / resolution.x)  * smoothstep(0.0, 1.0, r);
				float sum = 0.0;
				sum += sample(vec2(tc.x - 4.0*blur, tc.y), r) * 0.05;
				sum += sample(vec2(tc.x - 3.0*blur, tc.y), r) * 0.09;
				sum += sample(vec2(tc.x - 2.0*blur, tc.y), r) * 0.12;
				sum += sample(vec2(tc.x - 1.0*blur, tc.y), r) * 0.15;
				sum += center * 0.16;
				sum += sample(vec2(tc.x + 1.0*blur, tc.y), r) * 0.15;
				sum += sample(vec2(tc.x + 2.0*blur, tc.y), r) * 0.12;
				sum += sample(vec2(tc.x + 3.0*blur, tc.y), r) * 0.09;
				sum += sample(vec2(tc.x + 4.0*blur, tc.y), r) * 0.05;
				//smoothstepping unnecesary if the distance fading is done with vertex colors
				fColor = color * vec4(vec3(1.0), sum * smoothstep(1.0, 0.0, r));
				//fColor = color * vec4(vec3(1.0), sum);
				//hard shadows
				//fColor = color * vec4(vec3(1.0), center * smoothstep(1.0, 0.0, r));
			}
		)
	},
	{
		GL_NONE,
		NULL
	}
};
const char* shaderNames[] =
{
	"         Debug Shader",
	"Triangle Debug Shader",
	"         Color Shader",
	"        Sprite Shader",
	" Offset Sprite Shader",
	"   PointSprite Shader",
	"Colored Sprite Shader",
	"         Light Shader",
	"   Light Debug Shader",
	"      SP Color Shader",
	"     SP Sprite Shader",
	"     Shadowmap Shader",
	//"Cone Shadowmap Shader",
};
const SGL_ShaderInfo* builtInShaders[] =
{
	&debugShader,
	&triangleDebugShader,
	&colorShader,
	&spriteShader,
	&offsetSpriteShader,
	&pointSpriteShader,
	&coloredSpriteShader,
	&lightShader,
	&screenSpaceColorShader,
	&screenSpaceSpriteShader,
	&shadowMapShader,
};
const U32 shaderVertSizes[] =
{
	sizeof(SGL_Vec2),
	sizeof(SGL_Vec2),
	sizeof(SGL_Vec2) + sizeof(SGL_Color),
	sizeof(SGL_Vec2) + sizeof(SGL_Vec2),
	sizeof(SGL_Vec2) + sizeof(SGL_Vec2),
	sizeof(SGL_Vec2) + sizeof(SGL_Vec3),
	sizeof(SGL_Vec2) + sizeof(SGL_Vec2) + sizeof(SGL_Color),
	sizeof(SGL_Vec2) + sizeof(SGL_Vec2) + sizeof(SGL_Color),
	sizeof(SGL_Vec2) + sizeof(SGL_Color),
	sizeof(SGL_Vec2) + sizeof(SGL_Vec2),
	sizeof(SGL_Vec2) + sizeof(SGL_Vec2),
};
void colorShaderBindFunction(U32 VAO, U32 VBO, U32 shaderHandle)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glUseProgram(shaderHandle);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, shaderVertSizes[SGL_SHADER_COLOR], BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, shaderVertSizes[SGL_SHADER_COLOR], BUFFER_OFFSET(sizeof(SGL_Vec2)));

	glBindVertexArray(0);
	SGL_CHECK_GL_ERROR;
}
void spriteShaderBindFunction(U32 VAO, U32 VBO, U32 shaderHandle)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glUseProgram(shaderHandle);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, shaderVertSizes[SGL_SHADER_SPRITE], BUFFER_OFFSET(0));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, shaderVertSizes[SGL_SHADER_SPRITE], BUFFER_OFFSET(sizeof(SGL_Vec2)));

	glBindVertexArray(0);
	SGL_CHECK_GL_ERROR;
}
void coloredSpriteShaderBindFunction(U32 VAO, U32 VBO, U32 shaderHandle)
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
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
const void(*shaderFunctions[])(U32 VAO, U32 VBO, U32 shaderHandle) =
{
	NULL,
	NULL,
	&colorShaderBindFunction,
	&spriteShaderBindFunction,
	&spriteShaderBindFunction,
	&spriteShaderBindFunction,
	&coloredSpriteShaderBindFunction,
	&coloredSpriteShaderBindFunction,
	&colorShaderBindFunction,
	&spriteShaderBindFunction,
	&spriteShaderBindFunction,
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
U32 SGL_CreateProgram(SGL_ShaderInfo* shaders)
{
	if (shaders == NULL)
	{
		SDL_Log("Shader loading failed, ShaderInfo is null.");
		return 0;
	}
	GLuint program = glCreateProgram();
	//there is currently only 4 shader types but making space for 8 handles just to be sure
	GLuint shaderHandles[8] = { 0 };
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
	glBufferData(GL_UNIFORM_BUFFER, sizeof(SGL_Mat4), NULL, GL_STREAM_DRAW);
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
		//screen space shaders shouldnt be bound to the matrix block
		if (i < SGL_SHADER_SP_START)
		{
			BindShaderToMatrixBlock(rContext->shaders[i].handle);
		}
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
