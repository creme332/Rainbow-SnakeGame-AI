#include "Snake.h"
Snake::Snake(COORD p, int v) {
	this->pos = p;
	this->vel = v;
	len = 1;
	Body.push_back(p);
}

bool Snake::move_snake(char direction, int width, int height) { //returns 1 if there was a collision
	if (len >= (width-1) * (height-1))return 1; //game over if snake occupies entire board
	int x=pos.X, y=pos.Y; //new position of snake
	switch (direction) {
	case 'w': y = pos.Y - vel;break;
	case 's': y = pos.Y + vel;break;
	case 'a': x = pos.X - vel;break;
	case 'd': x = pos.X + vel;break;
	}
	//to show invalid move
	pos.Y = y;
	pos.X = x;
	//validate x, y
	if (y == height - 1 || x == width - 1 || x == 0 ||y == 0) {//went into grid
		return 1;
	}

	//to deal with 2-unit snake self-collision
	for (int i = 0;i < Body.size() - 1;i++) { //Body[lastindex] is previous pos of head. f
		if (Body[i].Y == y && Body[i].X == x) {return 1;}
	}

	//update head and tail of snake
	Body.push_back(pos);
	if (Body.size() > len) { //if body has grown in size
		Body.erase(Body.begin());
	}
	return 0;
}
vector <COORD> Snake::get_body() { return Body; }
void Snake::grow(){
	len+=5;
}
bool Snake ::eaten(COORD foodpos) {
	if (pos.Y == foodpos.Y && pos.X == foodpos.X)return 1;
	return 0;
}

COORD Snake::get_pos() {
	return pos;
}