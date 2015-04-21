#ifndef _Enemy_h_
#define _Enemy_h_

#include "Character.h"

struct GBFS_FILE;
struct Enemy;

typedef enum ENEMY_STATES 
{ 
    PATROL, 
    ATTACK 
} ENEMY_STATES;

typedef void*(*STATE_FN)(struct Enemy*, FIXED);

typedef struct Enemy
{
	Character base;
	STATE_FN current;
} Enemy;

void Enemy_construct(
	Enemy* self,
	Vector2 pos,
	const struct GBFS_FILE* dat,
	const char* image, 
	const char* pal, 
	const char* sprite,
	int palette
	);
	
void Enemy_load(Enemy* self, Vector2 pos, const struct GBFS_FILE* dat, const char* enemy);

void Enemy_update(Enemy* self, FIXED dt);

void Enemy_draw(Enemy* self, FIXED offset_x, FIXED offset_y);

void Enemy_destroy(Enemy* self);

#endif
