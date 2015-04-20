#ifndef _Enemy_h_
#define _Enemy_h_

#include "Character.h"

typdef enum ENEMY_STATES{ PATROL, ATTACK }ENEMY_STATES;
typedef void*(*stptr)(FIXED);
void* patrol_state(FIXED dt);
void* pursue_state(FIXED dt);

typedef struct Enemy 
{
	*stptr currentState;
	Character base;
} Enemy;

#endif
