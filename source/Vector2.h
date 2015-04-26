#ifndef _Vector2_h_
#define _Vector2_h_

#include "fixed.h"

typedef struct Vector2
{
	FIXED x, y;
} Vector2;

static inline Vector2 Vector2_create(FIXED x, FIXED y)
{
	Vector2 result = { x, y };
	return result;
}

static inline Vector2 Vector2_float(float x, float y)
{
	return Vector2_create(float_to_fx(x), float_to_fx(y));
}

static inline void Vector2_plus_equal(Vector2* lhs, Vector2 rhs)
{
	lhs->x += rhs.x;
	lhs->y += rhs.y;
}

static inline void Vector2_times_equal(Vector2* lhs, FIXED rhs)
{
	lhs->x = fx_mul(lhs->x, rhs);
	lhs->y = fx_mul(lhs->y, rhs);
}

static inline Vector2 Vector2_perp(const Vector2* vec)
{
	Vector2 result = { -vec->y, vec->x };
	return result;
}

static inline FIXED Vector2_mag_sq(const Vector2* vec)
{
	FIXED x = vec->x;
	FIXED y = vec->y;
	return fx_mul(x, x) + fx_mul(y, y);
}

static inline void Vector2_normalize(Vector2* vec)
{
	FIXED norm_sq = Vector2_mag_sq(vec);

	if(norm_sq == 0) return;

	Vector2_times_equal(vec, fx_inv_sqrt(norm_sq));
}

static inline void Vector2_negate_equal(Vector2* vec)
{
	vec->x = -vec->x;
	vec->y = -vec->y;
}

static inline Vector2 Vector2_negate(Vector2 vec)
{
	Vector2 result = { -vec.x, -vec.y };
	return result;
}

static inline Vector2 Vector2_add(Vector2 lhs, Vector2 rhs)
{
	Vector2 result = { lhs.x + rhs.x, lhs.y + rhs.y };
	return result;
}

static inline Vector2 Vector2_sub(Vector2 lhs, Vector2 rhs)
{
	Vector2 result = { lhs.x - rhs.x, lhs.y - rhs.y };
	return result;
}

static inline Vector2 Vector2_scalar_mult(Vector2 lhs, FIXED rhs)
{
	Vector2 result = { fx_mul(lhs.x, rhs) , fx_mul(lhs.y, rhs) };
	return result;
}

#endif
