#include "Sprite.h"

#include "oam_manager.h"

Sprite Sprite_new(int x, int y, u16 shape, u16 size, int gfxID)
{
	Sprite sprite = { x, y, obj_alloc(), gfxID };
	sprite.oam->attr0 = shape | OBJ_Y(y);
	sprite.oam->attr1 = size | OBJ_X(x);
	sprite.oam->attr2 = gfxID;
	return sprite;
}

void Sprite_draw(Sprite* self)
{
	OBJATTR* obj = self->oam;
	obj->attr0 = (obj->attr0 &~ 0x00FF) | OBJ_Y(self->y);
	obj->attr1 = (obj->attr1 &~ 0x01FF) | OBJ_X(self->x);
	obj->attr2 = self->gfxID * 8;
}

void Sprite_destroy(Sprite* self)
{
	obj_free(self->oam);
}
