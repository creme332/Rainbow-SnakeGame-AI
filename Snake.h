#pragma once
#include <Windows.h>
#include <vector>
class Snake
{
private :
	COORD pos;
	int len;
	int vel; //problem with vel. will skip food in between
	//vector<COORD> Body;
public:
	Snake(COORD pos, int vel);
	void move_snake(char direction);
	bool HasCollided(int width, int height);
	void grow();
	bool eaten(COORD foodpos);
	COORD get_pos();
};

