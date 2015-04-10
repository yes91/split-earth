#ifndef _Vector2_h_
#define _Vector2_h_

#include "fixed.h"

typedef struct Vector2
{
	FIXED x, y;
} Vector2;

Vector2 Vector2_create(FIXED x, FIXED y);

Vector2 Vector2_float(float x, float y);

Vector2 Vector2_perp(const Vector2* vec);

FIXED Vector2_mag_sq(const Vector2* vec);

void Vector2_normalize(Vector2* vec);

void Vector2_negate_equal(Vector2* vec);

Vector2 Vector2_negate(Vector2 vec);

Vector2 Vector2_add(Vector2 lhs, Vector2 rhs);

Vector2 Vector2_sub(Vector2 lhs, Vector2 rhs);

Vector2 Vector2_scalar_mult(Vector2 lhs, FIXED rhs);

void Vector2_plus_equal(Vector2* lhs, Vector2 rhs);

void Vector2_times_equal(Vector2* lhs, FIXED rhs);

#endif
