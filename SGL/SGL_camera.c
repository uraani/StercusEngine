#include "SGL.h"
SGL_Camera* SGL_CreateCamera(SGL_RenderContext* rContext, const unsigned int type, const unsigned int id, const float nearPlane, const float farPlane, const float FOWY, const float scale)
{
	SDL_memset(&rContext->cameras[id], 0, sizeof(SGL_Camera));
	rContext->cameras[id].rotation.x = 0.0f;
	rContext->cameras[id].rotation.y = 0.0f;
	rContext->cameras[id].rotation.z = 0.0f;
	rContext->cameras[id].rotation.w = 0.0f;
	rContext->cameras[id].position.x = 0.0f;
	rContext->cameras[id].position.y = 0.0f;
	rContext->cameras[id].position.z = 0.0f;
	rContext->cameras[id].nearPlane = nearPlane;
	rContext->cameras[id].farPlane = farPlane;
	rContext->cameras[id].FOWY = FOWY;
	rContext->cameras[id].scale = scale;
	rContext->cameras[id].camType = type;
	return &rContext->cameras[id];
}