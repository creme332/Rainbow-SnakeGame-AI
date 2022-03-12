#pragma once
#include <Windows.h>
#include <vector>
using namespace std;
class Snake
{
private :
	COORD pos; //x- col ans y-row
	int len; //initial length of snake?
	int vel; //problem with vel. will skip food in between
	vector <COORD> Body;
public:
	Snake(COORD pos, int vel);
	bool move_snake(char direction, int width, int height);
	void grow();
	bool eaten(COORD foodpos);
	COORD get_pos();
	vector <COORD> get_body();
};

