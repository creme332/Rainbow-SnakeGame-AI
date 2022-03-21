#include "Snake.h"
Snake::Snake(COORD headpos, int growth, int width, int height) {
	this->SnakeHeadPosition = headpos;
	SnakeLength = 1;
    GrowthRate = growth;
    GridHeight = height;
    GridWidth = width;
	Body.push_back(headpos);
}

bool Snake::move_snake(char direction) { //returns 1 if there was a collision

    if (direction == ERROR) return 1; //invalid direction
	if (SnakeLength >= (GridWidth -1) * (GridHeight -1))return 1; //game over if snake occupies entire board

	switch (direction) {
	case UP: SnakeHeadPosition.X--; break;
	case DOWN: SnakeHeadPosition.X++; break;
	case LEFT:  SnakeHeadPosition.Y--; break;
	case RIGHT:  SnakeHeadPosition.Y++; break;
	}

	//validate new position  of snake
	if (SnakeHeadPosition.X == GridHeight - 1 || SnakeHeadPosition.Y == GridWidth - 1 || SnakeHeadPosition.Y == 0 || SnakeHeadPosition.X == 0) {//collided with walls
		return 1;
	}

	//Snake self-collision. 
    //It is impossible for snake head to collide with itself.
    //Body[Body.size()-1] : head coordinates
	for (int i = 0;i < Body.size() - 1;i++) { 
        if (Body[i].X == SnakeHeadPosition.X && Body[i].Y == SnakeHeadPosition.Y) {
            return 1;
        }
	}

    //at this point, new position is valid.
	//update head and tail of snake
	Body.push_back(SnakeHeadPosition);
	if (Body.size() > SnakeLength) { //if body has grown in size
		Body.erase(Body.begin()); 
	}
	return 0;
}

std::vector <COORD> Snake::get_body() { return Body; }

void Snake::grow(){
	SnakeLength+=GrowthRate;
}

bool Snake ::eaten(COORD foodpos) {
	if (SnakeHeadPosition.Y == foodpos.Y && SnakeHeadPosition.X == foodpos.X)return 1;
	return 0;
}

COORD Snake::get_pos() {return SnakeHeadPosition;}

char Snake::minPath(COORD Destination) {
    int startcol = SnakeHeadPosition.Y;
    int startrow = SnakeHeadPosition.X;
    int finishcol = Destination.Y;
    int finishrow = Destination.X;

    std::map < std::pair<int, int>, int > distance = { {{startrow, startcol},0} }; //minimum distance from start to stored node(in terms of number of moves required)
    std::deque <std::pair<int, int>> NextNode = { {startrow, startcol} }; //head of queue is the next node to be checked
    std::map <std::pair<int, int>, std::pair<int, int>> parentnode = { {{startrow, startcol}, {-1,-1}} }; // {node, parent node}
    std::deque <std::pair<int, int>> shortestpath; //stores nodes along shortest path in (row,col) format

    std::vector <int> dx = { 1,0,-1,0 }; // translation horizontally
    std::vector <int> dy = { 0,1,0,-1 }; //translation vertically
    int x, y; // new coordinates (row,col) after moving 

    while (NextNode.size() != 0) {
        std:: pair<int, int> currentcoord = NextNode.front();
        NextNode.pop_front();

        for (int i = 0; i < 4;i++) {
            x = currentcoord.first + dx[i]; //row 
            y = currentcoord.second + dy[i]; //col

            if (x > 0 && x < GridHeight - 1 && y > 0
                && y < GridWidth - 1 && parentnode.count({ x,y }) == 0
                && NotSnakeBody(x, y))
            {
                NextNode.push_back({ x,y });
                parentnode[{x, y}] = { currentcoord.first, currentcoord.second };
                distance[{x, y}] = distance[currentcoord] + 1;

                if (x == finishrow && y == finishcol) {//shortest path known
                    int x0, y0;
                    while (x != -1) { // while start node not reached
                        shortestpath.push_front({ x, y });
                        x0 = x; y0 = y;
                        x = parentnode[{x0, y0}].first;
                        y = parentnode[{x0, y0}].second;
                    }
                    //0 th node of shortestpath is startnode and 1st node is our next node to be visited
                    int nextrow = shortestpath[1].first, nextcol = shortestpath[1].second;
                    if (nextrow == startrow) {
                        if (nextcol - startcol == 1) { return RIGHT; } //move right
                        return LEFT; //move left
                    }
                    if (nextcol == startcol) {
                        if (nextrow - startrow == 1) { return DOWN; }//move down
                        return UP;//move up
                    }
                }

            }

        }
    }
    return ERROR; //if path does not exist to Destination
}

bool Snake :: NotSnakeBody(int row, int col) {
    // Is cell at (row, col) not a snake body part ?
    for (int i = 0;i < Body.size();i++) {
        if (Body[i].Y == col && Body[i].X == row)return 0;
    }
    return 1;
}

char Snake::AI_Hamilton() {
    //GridWidth must be even and GridHeight must be odd
    int currentrow = SnakeHeadPosition.X;
    int currentcol = SnakeHeadPosition.Y;

    //return snake to start of cycle
    if (currentrow == 1) {
        if (currentcol == 1) {return DOWN;}
        else {return LEFT;}
    }

    if (currentrow == 2 && currentcol == GridWidth-2)return UP;

    if (currentcol % 2 == 1) { //move down or right
        if (currentrow == GridHeight - 2) { //turn 1 unit right
            return RIGHT;
        }
        else {return DOWN;}
    }
    else { //move up or right
        if (currentrow == 2) { //turn 1 unit right
            return RIGHT;
        }
        else {return UP;}
    }
}
char Snake::AI_BFS(COORD FoodPos) {
    int startcol = SnakeHeadPosition.Y;
    int startrow = SnakeHeadPosition.X;
      
    std::vector <int> dx = { 1,0,-1,0 }; // translation horizontally
    std::vector <int> dy = { 0,1,0,-1 }; //translation vertically

    char D1 = minPath(FoodPos); //direction required to go towards food in shortest steps
    if(D1!=ERROR)return D1;

    //if there's no current path to food, pick any valid path.
    int x, y;
    for (int i = 0;i < 4;i++) {
           x = startrow + dx[i]; // new row 
           y = startcol + dy[i]; // new col
            if (x > 0 && x < GridHeight - 1 && y > 0
                && y < GridWidth - 1 && NotSnakeBody(x, y)) {
                if (x == startrow) {
                    if (y- startcol == 1) { return RIGHT; } //move right
                    return LEFT; //move left
                }
                if (y == startcol) {
                    if (x - startrow == 1) { return DOWN; }//move down
                    return UP;//move up
                }
            }
    }
    //if there's no valid path, die.
    return DOWN;
}

