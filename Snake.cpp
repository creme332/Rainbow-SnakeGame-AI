#include "Snake.h"
Snake::Snake(COORD headpos, int width, int height) {
	this->SnakeHeadPosition = headpos;
	SnakeLength = 1;
    GridHeight = height;
    GridWidth = width;
	Body.push_back(headpos);
}

bool Snake::move_snake(char direction) { //returns 1 if there was a collision
	if (SnakeLength >= (GridWidth -1) * (GridHeight -1))return 1; //game over if snake occupies entire board

	int row = SnakeHeadPosition.X, col = SnakeHeadPosition.Y; //new position of snake
	switch (direction) {
	case UP: row --; break;
	case DOWN: row++; break;
	case LEFT: col--; break;
	case RIGHT: col++; break;
	}
    SnakeHeadPosition.X = row;
    SnakeHeadPosition.Y = col;

	//validate x, y
	if (row == GridHeight - 1 || col == GridWidth - 1 || col == 0 || row == 0) {//collided with walls
		return 1;
	}

	//Snake self-collision. 
    //Note it is impossible for the head of the snake to collide with the last part of its tail because they move at the same time.
    //It is also impossible for the head of the snake to collide with itself.
    //Body[0] : tail coordinates
    //Body[Body.size()-1] : head coordinates
	for (int i = 1;i < Body.size() - 1;i++) { 
        if (Body[i].X == row && Body[i].Y == col) {
            return 1;
        }
	}

    //at this point, new position is valid.
	//update head and tail of snake
	Body.push_back(SnakeHeadPosition);
	if (Body.size() > SnakeLength) { //if body has grown in size
		Body.erase(Body.begin()); // same effect as moving tail
	}
	return 0;
}

std::vector <COORD> Snake::get_body() { return Body; }

void Snake::grow(){
	SnakeLength+=5;
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

bool Snake::LastTailPart(int row, int col) {
    if(SnakeLength == 1)return 0;
    COORD tail = Body[Body.size() - 1];
    if (tail.X == row && tail.Y == col)return 1;
    return 0;
}
char Snake::AI(COORD FoodPos) {
    int startcol = SnakeHeadPosition.Y;
    int startrow = SnakeHeadPosition.X;

    std::vector <int> dx = { 1,0,-1,0 }; // translation horizontally
    std::vector <int> dy = { 0,1,0,-1 }; //translation vertically

    char D1 = minPath(FoodPos); //direction required to go towards food in shortest steps
    if(D1!=ERROR)return D1;
    int x, y;

    //find all possible moves from current position and evaluate them
    for (int i = 0;i < 4;i++) {
           x = startrow + dx[i]; // new row 
           y = startcol + dy[i]; // new col
            if (x > 0 && x < GridHeight - 1 && y > 0
                && y < GridWidth - 1 && NotSnakeBody(x, y) && !LastTailPart(x,y)) {

                if (x == startrow) {
                    if (y - startcol == 1) { return RIGHT; } //move right
                    return LEFT; //move left
                }
                if (y == startcol) {
                    if (x - startrow == 1) { return DOWN; }//move down
                    return UP;//move up
                }
            }
        }
    return RIGHT;

    return D1;
}

std::stack <std::pair<int, int>> Snake :: maxPath(COORD FoodPos) {// using brute force
    int startcol = SnakeHeadPosition.Y;
    int startrow = SnakeHeadPosition.X;
    int finishcol = FoodPos.Y;
    int finishrow = FoodPos.X;

    std::map < std::pair<int, int>, int > distance = { {{startrow, startcol},0} }; //distance from start to stored node(in terms of number of moves required)
    std::stack <std::pair<int, int>> NextNode; //next node to be checked
    std::stack <std::pair<int, int>> longestpath; //stores nodes along longest path in (row,col) format
    std::map <std::pair<int, int>, std::pair<int, int>> parentnode = { {{startrow, startcol}, {-1,-1}} }; // {node, parent node}

    std::vector <int> dx = { 1,0,-1,0 }; // translation horizontally
    std::vector <int> dy = { 0,1,0,-1 }; //translation vertically
    int x, y; // new coordinates (row,col) after moving 
    int currentdistance;
    int maxdistance = -1;

    NextNode.push({ startrow, startcol });

    while (NextNode.size() != 0) {
        std::pair<int, int> currentcoord = NextNode.top();
        currentdistance = distance[currentcoord];
        NextNode.pop();

        if (currentcoord.first == finishrow && currentcoord.second == finishcol) { //destination reached
            if (currentdistance > maxdistance) { //new longest path discovered
                maxdistance = currentdistance;
                longestpath.empty();
                int x0, y0;
                x = currentcoord.first;
                y = currentcoord.second;
                while (x != -1) { // while start node not reached, 
                    longestpath.push({ x, y });
                    x0 = x; y0 = y;
                    x = parentnode[{x0, y0}].first;
                    y = parentnode[{x0, y0}].second;
                }
            }
        }
        else {
            for (int i = 0; i < 4;i++) {
                x = currentcoord.first + dx[i]; //row 
                y = currentcoord.second + dy[i]; //col

                if (x > 0 && x < GridHeight - 1 && y > 0
                    && y < GridWidth - 1 && parentnode.count({ x,y }) == 0 //unvisited
                    && NotSnakeBody(x, y)) {
                    NextNode.push({ x,y });
                    if (currentdistance + 1 > distance[{x, y}]) {
                        distance[{x, y}]=currentdistance+1;
                    }
                    parentnode[{x, y}] = { currentcoord.first, currentcoord.second };
                }

            }
        }

    }
    //if path does not exist to food ?????
    return longestpath;
    
}