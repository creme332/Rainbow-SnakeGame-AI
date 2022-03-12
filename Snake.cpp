#include "Snake.h"
Snake::Snake(COORD p, int v) {
	this->pos = p;
	this->vel = v;
	len = 1;
}

void Snake::move_snake(char direction) {
	switch (direction) {
	case 'w': pos.Y-=vel;break;
	case 's': pos.Y+=vel;break;
	case 'a': pos.X-=vel;break;
	case 'd': pos.X+=vel;break;
	}
}
bool Snake::CollisionDetected() {
	return 1;
}
void Snake::grow(){
	len++;
}
bool Snake ::eaten(COORD foodpos) {
	if (pos.Y == foodpos.Y && pos.X == foodpos.X)return 1;
	return 0;
}

COORD Snake::get_pos() {
	return pos;
}