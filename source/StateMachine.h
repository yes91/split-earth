#ifndef _StateMachine_h_
#define _StateMachine_h_

#include "State.h"
#include "fixed.h"

struct GBFS_FILE;

typedef enum STATE_TYPE
{
	TITLE, OPTION, PLAY, GAMEOVER, NUM_STATES
} STATE_TYPE;

typedef struct StateMachine
{
	const struct GBFS_FILE* dat;
	STATE current;
	STATE* table;//[NUM_STATES];
} StateMachine;

void StateMachine_construct(
	StateMachine* self, 
	const struct GBFS_FILE* dat, 
	STATE* table, 
	STATE_TYPE initial
	);

void StateMachine_enter(StateMachine* self, STATE_TYPE state);

void StateMachine_update(StateMachine* self, FIXED dt);

void StateMachine_draw(StateMachine* self);

#endif