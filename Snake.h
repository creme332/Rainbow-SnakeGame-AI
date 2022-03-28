#pragma once
#include <Windows.h>
#include <vector>
#include <map>
#include <deque>
#include <stack>
#define LEFT 'a'
#define RIGHT 'd'
#define UP 'w'
#define DOWN 's'
#define INVALID 'x'
class Snake
{
private :
	COORD SnakeHeadPosition; //x : row ; y : col
	int SnakeLength; //initial length of snake?
	std::vector <COORD> Body;  //Body.X : row number and Body.Y : col number. Last position in Body is the head
	int GridWidth; //number of columns in grid
	int GridHeight; //number of rows in grid.
	int GrowthRate; //by how many units does snake grow after eating food
	char FreeDirection();
	int PythagorasDistance(COORD Destination); //pythagoras distance from snake head to another point

public:
	Snake(COORD headpos, int growth, int width, int height);
	bool move_snake(char direction);
	void grow();
	bool eaten(COORD foodpos);
	COORD get_pos();
	std::vector <COORD> get_body();
	char BFS(COORD FoodPos);
	bool NotSnakeBody(int row, int col); //is board[row][col] a snake body part?
	char AI_Hamilton();
	char AI_Hamilton_BFS(COORD FoodPos, int aggressiveness);
};

