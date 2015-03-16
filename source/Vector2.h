#ifndef _Vector2_h_
#define _Vector2_h_

#include "fixed.h"

typedef struct Vector2
{
	FIXED x, y;
} Vector2;

Vector2 Vector2_create(FIXED x, FIXED y);

Vector2 Vector2_perp(const Vector2* vec);

FIXED Vector2_mag_sq(const Vector2* vec);

void Vector2_normalize(Vector2* vec);

Vector2 Vector2_add(Vector2 lhs, Vector2 rhs);

Vector2 Vector2_sub(Vector2 lhs, Vector2 rhs);

Vector2 Vector2_scalar_mult(Vector2 lhs, int rhs);

void Vector2_plus_equal(Vector2* lhs, Vector2 rhs);

#endif