#include "Sprite.h"

#include "oam_manager.h"

void Sprite_construct(Sprite* self, int x, int y, u16 shape, u16 size, int gfxID)
{
	self->x = x;
	self->y = y;
	self->oam = obj_alloc();
	self->gfxID = gfxID;
	self->oam->attr0 = shape | OBJ_Y(y);
	self->oam->attr1 = size | OBJ_X(x);
	self->oam->attr2 = gfxID;
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
