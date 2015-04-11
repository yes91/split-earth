#ifndef _STATE_H_
#define _STATE_H_

#include "fixed.h"

struct GBFS_FILE;

typedef struct STATE {
	void (*construct)(const GBFS_FILE*);
	void (*update)(FIXED);
	void (*render)(void);
	void (*destroy)(void);
} STATE;

#endif
