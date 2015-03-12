#ifndef _Camera_h_
#define _Camera_h_

#include <gba_base.h>
#include <gba_types.h>
#include "Vector2.h"

struct Sprite;

enum CAMERA_BG
{
	CAM_BG0 = BIT(0),
	CAM_BG1 = BIT(1),
	CAM_BG2 = BIT(2),
	CAM_BG3 = BIT(3)
};

typedef struct Camera
{
	u32 bgs;
	Vector2 pos;
	Vector2 bounds;
	struct Sprite* target;
} Camera;

void Camera_construct(
	Camera* self, 
	u32 bgs, 
	int x, 
	int y, 
	Vector2 bounds, 
	struct Sprite* target
	);

void Camera_update(Camera* self);

void Camera_translate(Camera* self);

#endif