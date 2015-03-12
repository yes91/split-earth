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
	
	Vector2 offset = 
	{  
		self->target->pos.x - SCREEN_WIDTH/2 + 16, //TODO: Remove hardcoded half axes
		self->target->pos.y - SCREEN_HEIGHT/2 + 16
	};

	int max_x = self->bounds.x - SCREEN_WIDTH;
	int max_y = self->bounds.y - SCREEN_HEIGHT;

	offset.x = clamp(0, max_x, offset.x);
	offset.y = clamp(0, max_y, offset.y);

	self->pos = offset;
}

#define CAMERA_BG_TRANS(num) if(bgs & CAM_BG##num) { BG_OFFSET[num].x = self->pos.x; BG_OFFSET[num].y = self->pos.y; }

void Camera_translate(Camera* self)
{
	u32 bgs = self->bgs;
	
	CAMERA_BG_TRANS(0);
	CAMERA_BG_TRANS(1);
	CAMERA_BG_TRANS(2);
	CAMERA_BG_TRANS(3);
}