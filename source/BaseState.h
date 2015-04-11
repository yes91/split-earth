#ifndef _BASESTATE_H_
#define _BASESTATE_H_

#include "fixed.h"

struct GBFS_FILE;

typedef struct BaseState {
	void (*initState)(const GBFS_FILE*);
	void (*update)(FIXED);
	void (*render)(void);
	void (*destroy)(void);
}BaseState;

#endif
