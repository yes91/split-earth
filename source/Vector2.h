#ifndef _Vector2_h_
#define _Vector2_h_

typedef struct Vector2
{
	int x, y;
} Vector2;

int Vector2_mag_sq(const Vector2* vec);

Vector2 Vector2_scalar_mult(Vector2 lhs, int rhs);

void Vector2_plus_equal(Vector2* lhs, Vector2 rhs);

#endif