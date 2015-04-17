#ifndef _STATE_H_
#define _STATE_H_

#include "fixed.h"

struct GBFS_FILE;

struct StateMachine;

typedef struct STATE {
	void (*construct)(const struct GBFS_FILE*);
	void (*update)(struct StateMachine*, FIXED);
	void (*draw)(void);
	void (*destroy)(void);
} STATE;

#endif
