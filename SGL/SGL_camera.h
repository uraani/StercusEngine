#pragma once
#include "SGL_math.h"
struct SGL_RenderContext;
typedef enum SGL_CameraID
{
	SGL_CAMERA_GUI,
	SGL_CAMERA_ORTHO,
	SGL_CAMERA_PERSPECTIVE,
	SGL_CAMERA_CUSTOM0,
	SGL_CAMERA_CUSTOM1,
	SGL_CAMERA_CUSTOM2,
	SGL_CAMERA_CUSTOM3,
	SGL_CAMERA_CUSTOM4,
	SGL_CAMERA_COUNT
} SGL_CameraID;
typedef enum SGL_CameraType
{
	SGL_CAMERA_TYPE_UNUSED,
	SGL_CAMERA_TYPE_ORTHO,
	SGL_CAMERA_TYPE_PERSPECTIVE
} SGL_CameraType;
struct SGL_Camera
{
	SGL_Mat4 vPMatrix;
	SGL_Vec4 rotation;
	SGL_Vec3 position;
	float near;
	float far;
	float FOWY;
	float scale;
	Uint32 camType;
}typedef SGL_Camera;