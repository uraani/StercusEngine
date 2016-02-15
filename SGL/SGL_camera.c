#include "SGL.h"
SGL_Camera* SGL_CreateCamera(SGL_RenderContext* rContext, const Uint32 type, const Uint32 id, const float near, const float far, const float FOWY, const float scale)
{
	rContext->cameras[id].rotation.x = 0.0f;
	rContext->cameras[id].rotation.y = 0.0f;
	rContext->cameras[id].rotation.z = 0.0f;
	rContext->cameras[id].rotation.w = 0.0f;
	rContext->cameras[id].position.x = 0.0f;
	rContext->cameras[id].position.y = 0.0f;
	rContext->cameras[id].position.z = 0.0f;
	rContext->cameras[id].near = near;
	rContext->cameras[id].far = far;
	rContext->cameras[id].FOWY = FOWY;
	rContext->cameras[id].scale = scale;
	rContext->cameras[id].camType = type;
	if (type == SGL_CAMERA_PERSPECTIVE)
	{
		const float aspect = rContext->windowHalfSizef.x / rContext->windowHalfSizef.y;
		const float tanHalfFovy = tanf(FOWY / 2.0f);
		rContext->cameras[id].vPMatrix.m00 = 1.0f / (aspect * tanHalfFovy);
		rContext->cameras[id].vPMatrix.m11 = 1.0f / (tanHalfFovy);
		rContext->cameras[id].vPMatrix.m22 = -(far + near) / (far - near);
		rContext->cameras[id].vPMatrix.m23 = -1.0f;
		rContext->cameras[id].vPMatrix.m32 = -2.0f * far * near / (far - near);
	}
	else
	{
		const float left = rContext->windowHalfSizef.x * -scale;
		const float right = rContext->windowHalfSizef.x * scale;
		const float bottom = rContext->windowHalfSizef.y * -scale;
		const float top = rContext->windowHalfSizef.y * scale;
		rContext->cameras[id].vPMatrix.m00 = 2.0f / (right - left);
		rContext->cameras[id].vPMatrix.m11 = 2.0f / (top - bottom);
		rContext->cameras[id].vPMatrix.m22 = -2.0f / (far - near);
		rContext->cameras[id].vPMatrix.m30 = -(right + left) / (right - left);
		rContext->cameras[id].vPMatrix.m31 = -(top + bottom) / (top - bottom);
		rContext->cameras[id].vPMatrix.m32 = -(far + near) / (far - near);
	}
	return &rContext->cameras[id];
}
