#ifndef _GameObject_h_
#define _GameObject_h_

struct Sprite;

typedef enum Type
{
	PLAYER,
	ENEMY,
	ITEM
} Type;

typedef struct GameObject
{
	Type type;
	struct Sprite sprite;
} GameObject;

void GameObject_draw(GameObject* self);



#endif
