#include "Vector2.h"

int Vector2_mag_sq(const Vector2* vec)
{
	int x = vec->x;
	int y = vec->y;
	return x * x + y * y;
}

Vector2 Vector2_scalar_mult(Vector2 lhs, int rhs)
{
	Vector2 result = { lhs.x * rhs, lhs.y * rhs };
	return result;
}

void Vector2_plus_equal(Vector2* lhs, Vector2 rhs)
{
	lhs->x += rhs.x;
	lhs->y += rhs.y;
}