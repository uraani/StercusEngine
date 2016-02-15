#pragma once
//MATRIX INDEXES ARE ARRANGED LIKE THIS:
//[0 ][1 ][2 ][3 ]
//[4 ][5 ][6 ][7 ]
//[8 ][9 ][10][11]
//[12][13][14][15]
#if !defined(ANDROID)
__declspec(align(16))
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
	int x;
	int y;
}typedef SGL_Vec2i;
#if !defined(ANDROID)
__declspec(align(16))
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
//[0 ][1 ][2 ][3 ]
//[4 ][5 ][6 ][7 ]
//[8 ][9 ][10][11]
struct SGL_Mat3
{
	float m[9];

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
inline const SGL_Mat4 SM_Multiply(const SGL_Mat4* a, const SGL_Mat4* b)
{
	SGL_Mat4 r;
#if defined(ANDROID)
	float i00 = a->m00 * b->m00 + a->m01 * b->m10 + a->m02 * b->m20 + a->m03 * b->m30;
	float i01 = a->m00 * b->m01 + a->m01 * b->m11 + a->m02 * b->m21 + a->m03 * b->m31;
	float i02 = a->m00 * b->m02 + a->m01 * b->m12 + a->m02 * b->m22 + a->m03 * b->m32;
	float i03 = a->m00 * b->m03 + a->m01 * b->m13 + a->m02 * b->m23 + a->m03 * b->m33;
	float i10 = a->m10 * b->m00 + a->m11 * b->m10 + a->m12 * b->m20 + a->m13 * b->m30;
	float i11 = a->m10 * b->m01 + a->m11 * b->m11 + a->m12 * b->m21 + a->m13 * b->m31;
	float i12 = a->m10 * b->m02 + a->m11 * b->m12 + a->m12 * b->m22 + a->m13 * b->m32;
	float i13 = a->m10 * b->m03 + a->m11 * b->m13 + a->m12 * b->m23 + a->m13 * b->m33;
	float i20 = a->m20 * b->m00 + a->m21 * b->m10 + a->m22 * b->m20 + a->m23 * b->m30;
	float i21 = a->m20 * b->m01 + a->m21 * b->m11 + a->m22 * b->m21 + a->m23 * b->m31;
	float i22 = a->m20 * b->m02 + a->m21 * b->m12 + a->m22 * b->m22 + a->m23 * b->m32;
	float i23 = a->m20 * b->m03 + a->m21 * b->m13 + a->m22 * b->m23 + a->m23 * b->m33;
	float i30 = a->m30 * b->m00 + a->m31 * b->m10 + a->m32 * b->m20 + a->m33 * b->m30;
	float i31 = a->m30 * b->m01 + a->m31 * b->m11 + a->m32 * b->m21 + a->m33 * b->m31;
	float i32 = a->m30 * b->m02 + a->m31 * b->m12 + a->m32 * b->m22 + a->m33 * b->m32;
	float i33 = a->m30 * b->m03 + a->m31 * b->m13 + a->m32 * b->m23 + a->m33 * b->m33;
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