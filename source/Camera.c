#include "Camera.h"
#include "Sprite.h"
#include <gba_video.h>
#include "util.h"

void Camera_construct(
	Camera* self, 
	u32 bgs,
	int x, 
	int y, 
	Vector2 bounds, 
	Sprite* target
	)
{
	self->bgs = bgs;
	self->pos.x = x;
	self->pos.y = y;
	self->bounds = bounds;
	self->target = target;
}

void Camera_update(Camera* self)
{
	if (self->target == NULL) return;

	const FIXED half_axis = int_to_fx(16);
	const Vector2 half_vector = Vector2_create(half_axis, half_axis);
	
	Vector2 target_center = Vector2_add(self->target->pos, half_vector);

	const Vector2 screen_size = Vector2_create(int_to_fx(SCREEN_WIDTH), int_to_fx(SCREEN_HEIGHT));
	
	Vector2 offset = Vector2_sub(target_center, Vector2_scalar_mult(screen_size, float_to_fx(0.5f)));

	Vector2 max = Vector2_sub(self->bounds, screen_size);

	offset.x = clamp(0, max.x, offset.x);
	offset.y = clamp(0, max.y, offset.y);

	self->pos = offset;
}

#define CAMERA_BG_TRANS(num) if(self->bgs & CAM_BG##num) \
	{ BG_OFFSET[num].x = fx_to_int(self->pos.x); BG_OFFSET[num].y = fx_to_int(self->pos.y); }

void Camera_translate(Camera* self)
{
	CAMERA_BG_TRANS(0);
	CAMERA_BG_TRANS(1);
	CAMERA_BG_TRANS(2);
	CAMERA_BG_TRANS(3);
}