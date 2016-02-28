#pragma once
#include <smmintrin.h>
#include "SGL_types.h"
#ifndef _SGL_math_h
#define _SGL_math_h
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)
//MATRIX INDEXES ARE ARRANGED LIKE THIS:
//[0 ][1 ][2 ][3 ]
//[4 ][5 ][6 ][7 ]
//[8 ][9 ][10][11]
//[12][13][14][15]
#if !defined(ANDROID)
__declspec(align(16))
#else
#pragma message ("CREATE ARM NEON IMPLEMENTATIONS FOR VEC4")
#endif
struct SGL_Vec4
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
#if !defined(ANDROID)
		__m128 v;
#endif
	};
}typedef SGL_Vec4;
struct SGL_Vec3
{
	float x;
	float y;
	float z;
}typedef SGL_Vec3;
struct SGL_Vec2
{
	float x;
	float y;
}typedef SGL_Vec2;
struct SGL_Vec2i
{
	I32 x;
	I32 y;
}typedef SGL_Vec2i;
#if !defined(ANDROID)
__declspec(align(16))
#else
#pragma message ("CREATE ARM NEON IMPLEMENTATIONS FOR MAT4")
#endif
struct SGL_Mat4
{
	union 
	{
		struct
		{
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		struct
		{
			float m[16];
		};
		struct
		{
			SGL_Vec4 v0, v1, v2, v3;
		};
#if !defined(ANDROID)
		struct
		{

			__m128 m0, m1, m2, m3;
		};
#endif
	};
}typedef SGL_Mat4;
//MATRIX INDEXES ARE ARRANGED LIKE THIS:
//[0 ][1 ][2 ]
//[3 ][4 ][5 ]
//[6 ][7 ][8 ]
struct SGL_Mat3
{
	union
	{
		struct
		{
			float m00, m01, m02;
			float m10, m11, m12;
			float m20, m21, m22;
		};
		struct
		{
			float m[9];
		};
		struct
		{
			SGL_Vec3 v0, v1, v2;
		};
	};

}typedef SGL_Mat3;
inline const SGL_Mat4 SM_IdentityMat4()
{
	SGL_Mat4 r;
	r.m00 = 1.0f; r.m01 = 0.0f; r.m02 = 0.0f; r.m03 = 0.0f;
	r.m10 = 0.0f; r.m11 = 1.0f; r.m12 = 0.0f; r.m13 = 0.0f;
	r.m20 = 0.0f; r.m21 = 0.0f; r.m22 = 1.0f; r.m23 = 0.0f;
	r.m30 = 0.0f; r.m31 = 0.0f; r.m32 = 0.0f; r.m33 = 1.0f;
	return r;
}
inline const SGL_Mat4 SM_QuatToMat4(const SGL_Vec4* q)
{
	SGL_Mat4 r;
	const float qxx = q->x * q->x;
	const float qyy = q->y * q->y;
	const float qzz = q->z * q->z;
	const float qxz = q->x * q->z;
	const float qxy = q->x * q->y;
	const float qyz = q->y * q->z;
	const float qwx = q->w * q->x;
	const float qwy = q->w * q->y;
	const float qwz = q->w * q->z;
	r.m00 = 1.0f - 2.0f * (qyy + qzz);
	r.m01 = 2.0f * (qxy + qwz);
	r.m02 = 2.0f * (qxz - qwy);
	r.m03 = 0.0f;

	r.m10 = 2.0f * (qxy - qwz);
	r.m11 = 1.0f - 2.0f * (qxx + qzz);
	r.m12 = 2.0f * (qyz + qwx);
	r.m13 = 0.0f;

	r.m20 = 2.0f * (qxz + qwy);
	r.m21 = 2.0f * (qyz - qwx);
	r.m22 = 1.0f - 2.0f * (qxx + qyy);
	r.m23 = 0.0f;

	r.m30 = 0.0f;
	r.m31 = 0.0f;
	r.m32 = 0.0f;
	r.m33 = 1.0f;
	return r;

}
inline const SGL_Mat4 SM_M4Multiply(const SGL_Mat4* a, const SGL_Mat4* b)
{
	SGL_Mat4 r;
#if defined(ANDROID)
	const float i00 = a->m00 * b->m00 + a->m01 * b->m10 + a->m02 * b->m20 + a->m03 * b->m30;
	const float i01 = a->m00 * b->m01 + a->m01 * b->m11 + a->m02 * b->m21 + a->m03 * b->m31;
	const float i02 = a->m00 * b->m02 + a->m01 * b->m12 + a->m02 * b->m22 + a->m03 * b->m32;
	const float i03 = a->m00 * b->m03 + a->m01 * b->m13 + a->m02 * b->m23 + a->m03 * b->m33;
	const float i10 = a->m10 * b->m00 + a->m11 * b->m10 + a->m12 * b->m20 + a->m13 * b->m30;
	const float i11 = a->m10 * b->m01 + a->m11 * b->m11 + a->m12 * b->m21 + a->m13 * b->m31;
	const float i12 = a->m10 * b->m02 + a->m11 * b->m12 + a->m12 * b->m22 + a->m13 * b->m32;
	const float i13 = a->m10 * b->m03 + a->m11 * b->m13 + a->m12 * b->m23 + a->m13 * b->m33;
	const float i20 = a->m20 * b->m00 + a->m21 * b->m10 + a->m22 * b->m20 + a->m23 * b->m30;
	const float i21 = a->m20 * b->m01 + a->m21 * b->m11 + a->m22 * b->m21 + a->m23 * b->m31;
	const float i22 = a->m20 * b->m02 + a->m21 * b->m12 + a->m22 * b->m22 + a->m23 * b->m32;
	const float i23 = a->m20 * b->m03 + a->m21 * b->m13 + a->m22 * b->m23 + a->m23 * b->m33;
	const float i30 = a->m30 * b->m00 + a->m31 * b->m10 + a->m32 * b->m20 + a->m33 * b->m30;
	const float i31 = a->m30 * b->m01 + a->m31 * b->m11 + a->m32 * b->m21 + a->m33 * b->m31;
	const float i32 = a->m30 * b->m02 + a->m31 * b->m12 + a->m32 * b->m22 + a->m33 * b->m32;
	const float i33 = a->m30 * b->m03 + a->m31 * b->m13 + a->m32 * b->m23 + a->m33 * b->m33;
	r.m00 = i00;
	r.m01 = i01;
	r.m02 = i02;
	r.m03 = i03;
	r.m10 = i10;
	r.m11 = i11;
	r.m12 = i12;
	r.m13 = i13;
	r.m20 = i20;
	r.m21 = i21;
	r.m22 = i22;
	r.m23 = i23;
	r.m30 = i30;
	r.m31 = i31;
	r.m32 = i32;
	r.m33 = i33;
#else
	const __m128 av = _mm_load_ps((float*)&b->m00);
	const __m128 bv = _mm_load_ps((float*)&b->m10);
	const __m128 cv = _mm_load_ps((float*)&b->m20);
	const __m128 dv = _mm_load_ps((float*)&b->m30);

	__m128 t1, t2;

	t1 = _mm_set1_ps(a->m00);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m01);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m02);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);
	t1 = _mm_set1_ps(a->m03);
	t2 = _mm_add_ps(_mm_mul_ps(dv, t1), t2);

	_mm_store_ps(&r.m00, t2);

	t1 = _mm_set1_ps(a->m10);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m11);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m12);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);
	t1 = _mm_set1_ps(a->m13);
	t2 = _mm_add_ps(_mm_mul_ps(dv, t1), t2);

	_mm_store_ps(&r.m10, t2);

	t1 = _mm_set1_ps(a->m20);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m21);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m22);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);
	t1 = _mm_set1_ps(a->m23);
	t2 = _mm_add_ps(_mm_mul_ps(dv, t1), t2);

	_mm_store_ps(&r.m20, t2);

	t1 = _mm_set1_ps(a->m30);
	t2 = _mm_mul_ps(av, t1);
	t1 = _mm_set1_ps(a->m31);
	t2 = _mm_add_ps(_mm_mul_ps(bv, t1), t2);
	t1 = _mm_set1_ps(a->m32);
	t2 = _mm_add_ps(_mm_mul_ps(cv, t1), t2);
	t1 = _mm_set1_ps(a->m33);
	t2 = _mm_add_ps(_mm_mul_ps(dv, t1), t2);

	_mm_store_ps(&r.m30, t2);
#endif
	return r;

}
inline const SGL_Vec4 SM_V4Multiply(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	r.w = a->w - b->w;
#else
	r.v = _mm_mul_ps(a->v, b->v);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4Subtract(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = a->x - b->x;
	r.y = a->y - b->y;
	r.z = a->z - b->z;
	r.w = a->w - b->w;	
#else
	r.v = _mm_sub_ps(a->v, b->v);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4Add(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = a->x + b->x;
	r.y = a->y + b->y;
	r.z = a->z + b->z;
	r.w = a->w + b->w;
#else
	//approx 10x faster
	r.v = _mm_add_ps(a->v, b->v);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4Negate(const SGL_Vec4* a)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = -a->x;
	r.y = -a->y;
	r.z = -a->z;
	r.w = -a->w;
#else
	//approx 7x faster
	r.v = _mm_sub_ps(_mm_set1_ps(0.0f), a->v);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4Normalize(const SGL_Vec4* a)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	float m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
	r.x = a->x / m;
	r.y = a->y / m;
	r.z = a->z / m;
	r.w = 0.0f;
#else
	//The immediate bits 4-7 of mask determine which of the corresponding source operand pairs are to be multiplied. 
	//Bits 0-3 determine whether the dot product result will be written.
	//If a mask bit is 0, the corresponding product result or written value is +0.0.
	//0x7F = 0111 1111, so the 'w'-component is discarded
	__m128 dotP = _mm_dp_ps(a->v, a->v, 0x7F);
	//calculate the reciprocal square root of dotP, 1 / sqrt(dotP)
	dotP = _mm_rsqrt_ps(dotP);
	// vec * rsqrt(dot(vec, vec))
	r.v = _mm_mul_ps(a->v, dotP);
#endif
	return r;
}
inline const SGL_Vec4 SM_V4NormalizePrecise(const SGL_Vec4* a)
{
	SGL_Vec4 r;
	float m = sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
	r.x = a->x / m;
	r.y = a->y / m;
	r.z = a->z / m;
	r.w = 0.0f;
	return r;
}
inline const float SM_Dot(const SGL_Vec4* a, const SGL_Vec4* b)
{
//#if defined(ANDROID)
	return a->x * b->x + a->y * b->y + a->z * b->z;
/*#else
	SGL_Vec4 r;
	//this function is way slower than the code above, SSE4 isnt supported?
	r.v = _mm_dp_ps(a->v,b->v, 0x7F);
	return r.x;
#endif*/
}
inline const SGL_Vec4 SM_V4Cross(const SGL_Vec4* a, const SGL_Vec4* b)
{
	SGL_Vec4 r;
#if defined(ANDROID)
	r.x = a->y * b->z - a->z * b->y;
	r.y = a->z * b->x - a->x * b->z;
	r.z = a->x * b->y - a->y * b->x;
	r.w = 0.0f;
#else
	//yields very little perf 1.1x faster max
	r.v = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(a->v, a->v, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b->v, b->v, _MM_SHUFFLE(3, 1, 0, 2))),
		_mm_mul_ps(_mm_shuffle_ps(a->v, a->v, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b->v, b->v, _MM_SHUFFLE(3, 0, 2, 1)))
		);
	//_mm_load_ps((float*)&a->x)
#endif
	return r;
}
inline const SGL_Mat4 SM_LookAt(const SGL_Vec4* eye, const SGL_Vec4* center, const SGL_Vec4* up)
{
	SGL_Mat4 r;
	const SGL_Vec4 t0 = SM_V4Subtract(center, eye);
	const SGL_Vec4 f = SM_V4NormalizePrecise(&t0);
	const SGL_Vec4 t1 = SM_V4Cross(&f, up);
	const SGL_Vec4 s = SM_V4NormalizePrecise(&t1);
	const SGL_Vec4 u = SM_V4Cross(&s, &f);
	r.m00 = s.x;
	r.m10 = s.y;
	r.m20 = s.z;

	r.m01 = u.x;
	r.m11 = u.y;
	r.m21 = u.z;

	r.m02 = -f.x;
	r.m12 = -f.y;
	r.m22 = -f.z;

	r.m30 = -SM_Dot(&s, eye);
	r.m31 = -SM_Dot(&u, eye);
	r.m32 = SM_Dot(&f, eye);

	r.m03 = 0.0f;
	r.m13 = 0.0f;
	r.m23 = 0.0f;
	r.m33 = 1.0f;
	return r;
}
inline const SGL_Mat4 SM_Rotate(const SGL_Mat4* m, float angle, const SGL_Vec4* v)
{
	SGL_Mat4 rot;
	SGL_Mat4 r;
	float const a = angle;
	float const c = SDL_cos(a);
	float const s = SDL_sin(a);
	SGL_Vec4 axis = SM_V4NormalizePrecise(v);
	SGL_Vec4 temp = { 1.0f - c, 1.0f - c, 1.0f - c, 0.0f };
	temp = SM_V4Multiply(&temp, &axis);

	rot.m00 = c + temp.x * axis.x;
	rot.m01 = 0.0f + temp.x * axis.y + s * axis.z;
	rot.m02 = 0.0f + temp.x * axis.z - s * axis.y;
	rot.m03 = 0.0f;

	rot.m10 = 0.0f + temp.y * axis.x - s * axis.z;
	rot.m11 = c + temp.y * axis.y;
	rot.m12 = 0.0f + temp.y * axis.z + s * axis.x;
	rot.m13 = 0.0f;

	rot.m20 = 0.0f + temp.z * axis.x + s * axis.y;
	rot.m21 = 0.0f + temp.z * axis.y - s * axis.x;
	rot.m22 = c + temp.z * axis.z;
	rot.m23 = 0.0f;

	rot.m30 = 0.0f;
	rot.m31 = 0.0f;
	rot.m32 = 0.0f;
	rot.m33 = 1.0f;
	r = SM_M4Multiply(m, &rot);
	/*T const a = angle;
		T const c = cos(a);
		T const s = sin(a);

		tvec3<T, P> axis(normalize(v));
		tvec3<T, P> temp((T(1) - c) * axis);

		tmat4x4<T, P> Rotate(uninitialize);
		Rotate[0][0] = c + temp[0] * axis[0];
		Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
		Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

		Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
		Rotate[1][1] = c + temp[1] * axis[1];
		Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

		Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
		Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
		Rotate[2][2] = c + temp[2] * axis[2];

		tmat4x4<T, P> Result(uninitialize);
		Result[0] = m[0] * Rotate[0][0] + m[1] * Rotate[0][1] + m[2] * Rotate[0][2];
		Result[1] = m[0] * Rotate[1][0] + m[1] * Rotate[1][1] + m[2] * Rotate[1][2];
		Result[2] = m[0] * Rotate[2][0] + m[1] * Rotate[2][1] + m[2] * Rotate[2][2];
		Result[3] = m[3];*/
	return r;
}
inline const SGL_Mat4 SM_Perspective(const float FOWY,const float aspect,const float nearPlane,const float farPlane)
{
	SGL_Mat4 r;
	const float tanHalfFovy = SDL_tanf(FOWY / 2.0f);

	r.m00 = 1.0f / (aspect * tanHalfFovy);
	r.m01 = 0.0f;
	r.m02 = 0.0f;
	r.m03 = 0.0f;

	r.m10 = 0.0f;
	r.m11 = 1.0f / (tanHalfFovy);
	r.m12 = 0.0f;
	r.m13 = 0.0f;

	r.m20 = 0.0f;
	r.m21 = 0.0f;
	r.m22 = -(farPlane + nearPlane) / (farPlane - nearPlane);
	r.m23 = -1.0f;

	r.m30 = 0.0f;
	r.m31 = 0.0f;
	r.m32 = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
	r.m33 = 0.0f;
	return r;
}
#endif