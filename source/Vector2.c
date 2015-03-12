#include "Vector2.h"

int Vector2_mag_sq(const Vector2* vec)
{
	int x = vec->x;
	int y = vec->y;
	return x * x + y * y;
}