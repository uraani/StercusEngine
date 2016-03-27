#include "SGL_texture.h"
#include "SGL_window.h"
#include "GL\glew.h"
SGL_TexParams SGL_ParamsLinear = { GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, { 1.0f, 0.0f, 1.0f, 1.0f } };
SGL_TexParams SGL_ParamsNearest = { GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST,{ 1.0f, 0.0f, 1.0f, 1.0f } };
void SGL_CreateTextures(SGL_Tex2D* textures, SGL_TexParams* params, U32 count)
{
	U32* texHandles = SDL_malloc(sizeof(U32)*count);
	glGenTextures(count, texHandles);
	for (U32 i = 0; i < count; i++)
	{
		textures[i].handle = texHandles[i];
		glBindTexture(GL_TEXTURE_2D, texHandles[i]);

		//TODO: Only change these parameters if needed
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params[i].wrapModeS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params[i].wrapModeT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params[i].minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params[i].magFilter);

		if (params[i].wrapModeS == GL_CLAMP_TO_BORDER || params[i].wrapModeT == GL_CLAMP_TO_BORDER)
		{
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, params[i].borderColor);
		}
		glTexImage2D(GL_TEXTURE_2D, 0, textures[i].type, textures[i].width, textures[i].height, 0, textures[i].type, GL_UNSIGNED_BYTE, textures[i].data);
		SDL_free(textures[i].data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	SDL_free(texHandles);
}
