#include "Food.h"
#include <cstdlib>
Food::Food() {
	GenerateFood();
}

COORD Food::get_pos() {
return pos;
}
void Food::GenerateFood() { //generate food inside grid but not on snake
	pos.X = 0;
}