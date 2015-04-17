#include "StateMachine.h"
#include <stddef.h>
#include <gbfs.h>
#include <string.h>

void StateMachine_construct(
	StateMachine* self, 
	const struct GBFS_FILE* dat, 
	STATE* table, 
	STATE_TYPE initial
	)
{
	self->dat = dat;
	self->table = table;
	self->current = self->table[initial];
	self->current.construct(self->dat);
}

void StateMachine_enter(StateMachine* self, STATE_TYPE state)
{
	self->current.destroy();
	self->current = self->table[state];
	self->current.construct(self->dat);
}

void StateMachine_update(StateMachine* self, FIXED dt)
{
	self->current.update(self, dt);
}

void StateMachine_draw(StateMachine* self)
{
	self->current.draw();
}