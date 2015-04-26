#ifndef _PlayState_h_
#define _PlayState_h_

#include "State.h"
#include "Sprite.h"

extern const STATE play_state;
int check_collision(Sprite* a, Sprite* b);
void handle_collision(Sprite* a, Sprite* b);

#endif
