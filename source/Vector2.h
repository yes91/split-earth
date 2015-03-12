#ifndef _Vector2_h_
#define _Vector2_h_

typedef struct Vector2
{
	int x, y;
} Vector2;

int Vector2_mag_sq(const Vector2* vec);

#endif