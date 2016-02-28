#include "SGL_camera.h"
//#include "SDL.h"
SGL_Camera* SGL_CreateCamera(SGL_RenderContext* rContext, const unsigned int type, const unsigned int id, const float nearPlane, const float farPlane, const float FOWY, const float scale)
{
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
	if (type == SGL_CAMERA_PERSPECTIVE)
	{
		const float aspect = rContext->windowHalfSizef.x / rContext->windowHalfSizef.y;
		const float tanHalfFovy = SDL_tanf(FOWY / 2.0f);

		rContext->cameras[id].vPMatrix.m00 = 1.0f / (aspect * tanHalfFovy);
		rContext->cameras[id].vPMatrix.m01 = 0.0f;
		rContext->cameras[id].vPMatrix.m02 = 0.0f;
		rContext->cameras[id].vPMatrix.m03 = 0.0f;

		rContext->cameras[id].vPMatrix.m10 = 0.0f;
		rContext->cameras[id].vPMatrix.m11 = 1.0f / (tanHalfFovy);
		rContext->cameras[id].vPMatrix.m12 = 0.0f;
		rContext->cameras[id].vPMatrix.m13 = 0.0f;

		rContext->cameras[id].vPMatrix.m20 = 0.0f;
		rContext->cameras[id].vPMatrix.m21 = 0.0f;
		rContext->cameras[id].vPMatrix.m22 = -(farPlane + nearPlane) / (farPlane - nearPlane);
		rContext->cameras[id].vPMatrix.m23 = -1.0f;

		rContext->cameras[id].vPMatrix.m30 = 0.0f;
		rContext->cameras[id].vPMatrix.m31 = 0.0f;
		rContext->cameras[id].vPMatrix.m32 = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
		rContext->cameras[id].vPMatrix.m33 = 0.0f;
	}
	else
	{
		const float left = rContext->windowHalfSizef.x * -scale;
		const float right = rContext->windowHalfSizef.x * scale;
		const float bottom = rContext->windowHalfSizef.y * -scale;
		const float top = rContext->windowHalfSizef.y * scale;
		rContext->cameras[id].vPMatrix.m00 = 2.0f / (right - left);
		rContext->cameras[id].vPMatrix.m01 = 0.0f;
		rContext->cameras[id].vPMatrix.m02 = 0.0f;
		rContext->cameras[id].vPMatrix.m03 = 0.0f;

		rContext->cameras[id].vPMatrix.m10 = 0.0f;
		rContext->cameras[id].vPMatrix.m11 = 2.0f / (top - bottom);
		rContext->cameras[id].vPMatrix.m12 = 0.0f;
		rContext->cameras[id].vPMatrix.m13 = 0.0f;

		rContext->cameras[id].vPMatrix.m20 = 0.0f;
		rContext->cameras[id].vPMatrix.m21 = 0.0f;
		rContext->cameras[id].vPMatrix.m22 = -2.0f / (farPlane - nearPlane);
		rContext->cameras[id].vPMatrix.m23 = -1.0f;

		rContext->cameras[id].vPMatrix.m30 = -(right + left) / (right - left);
		rContext->cameras[id].vPMatrix.m31 = -(top + bottom) / (top - bottom);
		rContext->cameras[id].vPMatrix.m32 = -(farPlane + nearPlane) / (farPlane - nearPlane);
		rContext->cameras[id].vPMatrix.m33 = 0.0f;
	}
	return &rContext->cameras[id];
}
SGL_UpdateCameras(SGL_RenderContext* rContext)
{
	unsigned int i;
	for (i = 0; i < SGL_CAMERA_COUNT; i++)
	{
		if (rContext->cameras[i].camType == SGL_CAMERA_PERSPECTIVE)
		{
			const float aspect = rContext->windowHalfSizef.x / rContext->windowHalfSizef.y;
			const float tanHalfFovy = SDL_tanf(rContext->cameras[i].FOWY / 2.0f);
			rContext->cameras[i].vPMatrix.m00 = 1.0f / (aspect * tanHalfFovy);
			rContext->cameras[i].vPMatrix.m11 = 1.0f / (tanHalfFovy);
			rContext->cameras[i].vPMatrix.m22 = -(rContext->cameras[i].farPlane + rContext->cameras[i].nearPlane) / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			rContext->cameras[i].vPMatrix.m23 = -1.0f;
			rContext->cameras[i].vPMatrix.m32 = -2.0f * rContext->cameras[i].farPlane * rContext->cameras[i].nearPlane / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
		}
		else
		{
			const float left = rContext->windowHalfSizef.x * -rContext->cameras[i].scale;
			const float right = rContext->windowHalfSizef.x * rContext->cameras[i].scale;
			const float bottom = rContext->windowHalfSizef.y * -rContext->cameras[i].scale;
			const float top = rContext->windowHalfSizef.y * rContext->cameras[i].scale;
			rContext->cameras[i].vPMatrix.m00 = 2.0f / (right - left);
			rContext->cameras[i].vPMatrix.m11 = 2.0f / (top - bottom);
			rContext->cameras[i].vPMatrix.m22 = -2.0f / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
			rContext->cameras[i].vPMatrix.m30 = -(right + left) / (right - left);
			rContext->cameras[i].vPMatrix.m31 = -(top + bottom) / (top - bottom);
			rContext->cameras[i].vPMatrix.m32 = -(rContext->cameras[i].farPlane + rContext->cameras[i].nearPlane) / (rContext->cameras[i].farPlane - rContext->cameras[i].nearPlane);
		}
	}
}