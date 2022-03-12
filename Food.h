#pragma once
#include <Windows.h>
#include <cstdlib>
class Food
{
private :
	COORD pos;
public:
	Food();
	COORD get_pos();
	void GenerateFood();

};

