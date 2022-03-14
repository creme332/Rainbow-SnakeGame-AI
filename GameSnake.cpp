#include <iostream>
#include <cstring>
#include <conio.h> 
#include <string>
#include "Snake.h"
#include <Windows.h>
#include <thread>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <deque>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define HEIGHT 25
#define WIDTH 40
#define VERTICAL_WALL '|'
#define HORIZONTAL_WALL '#'
#define EMPTY ' '
#define FOOD '*'
#define SNAKEHEAD '@'
#define SNAKEBODY 'o'  
using namespace std;

char TerminalGrid[HEIGHT][WIDTH]; //board currently displayed on terminal
COORD grid = { WIDTH / 2, HEIGHT / 2 };
Snake mySnake(grid, 1);
COORD FoodPos; //position of food on board
int k = -1; //color counter
string Color(char c) { // color a character
    int color[] = { 196,220,226,10,39,129 }; //list of colors
    if (c == SNAKEHEAD) return  "\033[48;5;196m\033[38;5;232m \033[0m"; //green block for head
    if (c == SNAKEBODY) {
        // int k = rand() % (sizeof(color) / sizeof(color[0])); //pick a random color for snake body
        k = (k + 1) % (sizeof(color) / sizeof(color[0]));
        return  "\033[48;5;" + to_string(color[k]) + "m\033[38;5;232m \033[0m";
    }
    if (c == HORIZONTAL_WALL || c == VERTICAL_WALL)return  "\033[48;5;15m\033[38;5;232m \033[0m";
    if (c == FOOD) return  "\033[48;5;14m\033[38;5;232m \033[0m"; //red block for food
    string output = ""; output += c;
    return output;
}
bool NotSnakeBody(int row, int col) {
    // Is cell at (row, col) not a snake body part ?
    vector <COORD> snakebody = mySnake.get_body();
    for (int i = 0;i < snakebody.size();i++) {
        if (snakebody[i].X == col && snakebody[i].Y == row)return 0;
    }
    return 1;
}
void GenerateFood() {
    // col in [1, width - 2] and row in [1, height - 2]
    FoodPos.X = (rand() % (WIDTH - 2)) + 1;
    FoodPos.Y = (rand() % (HEIGHT - 2)) + 1;

    while (!NotSnakeBody(FoodPos.Y, FoodPos.X)) { //spawn food in a position not occupied by snake
        FoodPos.X = (rand() % (WIDTH - 2)) + 1;
        FoodPos.Y = (rand() % (HEIGHT - 2)) + 1;
    }

}
void hidecursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}
void setCursorPosition(const int row, const int col)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = { (SHORT)col, (SHORT)row };
    SetConsoleCursorPosition(hOut, coord);
}
void InitialiseTerminal() {

    GenerateFood();
    COORD SnakeHeadPosition = mySnake.get_pos();
    //first initialise TerminalGrid 
    for (int row = 0;row < HEIGHT;row++) { //ADD snake head here!!!!
        TerminalGrid[row][0] = VERTICAL_WALL;
        for (int col = 1;col < WIDTH - 1;col++) {
            if (row == 0 || row == HEIGHT - 1)TerminalGrid[row][col] = HORIZONTAL_WALL;
            else {
                if (row == SnakeHeadPosition.Y && col == SnakeHeadPosition.X) { TerminalGrid[row][col] = SNAKEHEAD; }
                else {
                    if (row == FoodPos.Y && col == FoodPos.X) { TerminalGrid[row][col] = FOOD; }
                    else { TerminalGrid[row][col] = EMPTY; }
                }
            }
        }
        TerminalGrid[row][WIDTH - 1] = VERTICAL_WALL;
    }

    //output to terminal
    for (int row = 0;row < HEIGHT;row++) {
        for (int col = 0;col < WIDTH;col++) {
            cout << Color(TerminalGrid[row][col]);
        }
        cout << "\n";
    }
}
void UpdateBoard() {
    COORD SnakeHeadPosition = mySnake.get_pos();
    vector <COORD> snakebody = mySnake.get_body();
    setCursorPosition(HEIGHT, 0);
    COORD currentpos;
    //create a new board
    char newBoard[HEIGHT][WIDTH];

    for (int row = 0;row < HEIGHT;row++) {
        newBoard[row][0] = VERTICAL_WALL;
        for (int col = 1;col < WIDTH - 1;col++) {
            if (row == 0 || row == HEIGHT - 1)newBoard[row][col] = HORIZONTAL_WALL;
            else {
                if (SnakeHeadPosition.Y == row && SnakeHeadPosition.X == col) { newBoard[row][col] = SNAKEHEAD; }
                else {
                    if (row == FoodPos.Y && col == FoodPos.X) { newBoard[row][col] = FOOD; }
                    else {
                        bool body = 0;
                        for (auto i : snakebody) {
                            if (i.Y == row && i.X == col) body = 1; newBoard[row][col] = SNAKEBODY;
                        }
                        if (!body) {
                            newBoard[row][col] = EMPTY; //if cell is not wall, food nor snake, it is empty
                        }
                    }
                }
            }
        }
        newBoard[row][WIDTH - 1] = VERTICAL_WALL;
    }
    //compare with previous board currently on screen and add changes directly to terminal
    //no need to check boundaries since they never change
    for (int row = 0;row < HEIGHT;row++) {
        for (int col = 0;col < WIDTH;col++) {
            if (newBoard[row][col] != TerminalGrid[row][col] ||
                newBoard[row][col] == VERTICAL_WALL ||
                newBoard[row][col] == HORIZONTAL_WALL ||
                newBoard[row][col] == FOOD) { //for when terminal is resized
                setCursorPosition(row, col);
                std::cout << Color(newBoard[row][col]);
            }
        }
    }
    std::cout.flush();
    std::memcpy((char*)TerminalGrid, (char const*)newBoard, HEIGHT * WIDTH);
}
char AI() { //bfs
    COORD SnakeHeadPosition = mySnake.get_pos();
    int startcol = SnakeHeadPosition.X;
    int startrow = SnakeHeadPosition.Y;
    int finishcol = FoodPos.X;
    int finishrow = FoodPos.Y;
    //if (startrow == finishrow && startcol == finishcol) return 0;
    map <pair<int, int>, int> distance = { {{startrow, startcol},0} }; //minimum distance from start to stored node(in terms of number of moves required)
    deque <pair<int, int>> NextNode = { {startrow, startcol} }; //head of queue is the next node to be checked
    map <pair<int, int>, pair<int, int>> parentnode = { {{startrow, startcol}, {-1,-1}} }; // {node, parent node}
    deque <pair<int, int>> shortestpath; //stores nodes along shortest path in (row,col) format

    vector <int> dx = { 1,0,-1,0 }; // translation horizontally
    vector <int> dy = { 0,1,0,-1 }; //translation vertically
    int x, y; // new coordinates (row,col) after moving 

    while (NextNode.size() != 0) {
        pair<int, int> currentcoord = NextNode.front();
        NextNode.pop_front();

        for (int i = 0; i < 4;i++) {
            x = currentcoord.first + dx[i]; //row 
            y = currentcoord.second + dy[i]; //col

            if (x > 0 && x < HEIGHT - 1 && y > 0
                && y < WIDTH - 1 && parentnode.count({ x,y }) == 0
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
                        if (nextcol - startcol == 1) { return 'd'; } //move right
                        return 'a'; //move left
                    }
                    if (nextcol == startcol) {
                        if (nextrow - startrow == 1) { return 's'; }//move down
                        return 'w';//move up
                    }
                }

            }

        }
    }
    //if no path to food is currently available, move to a free position
    for (int i = 0;i < 4;i++) {
        x = startrow + dx[i]; // new row 
        y = startcol + dy[i]; // new col
        if (x > 0 && x < HEIGHT - 1 && y > 0 //validation
            && y < WIDTH - 1 && NotSnakeBody(x, y)) {
            //free position found
            if (x == startrow) {
                if (y - startcol == 1) { return 'd'; } //move right
                return 'a'; //move left
            }
            if (y == startcol) {
                if (x - startrow == 1) { return 's'; }//move down
                return 'w';//move up
            }
        }
    }
    return 'd';
}
int main() {
    
    srand(time(NULL)); //seed for random function
    int score = 0;
    char Direction = 'd'; //initially move right
    bool HasCollided = 0; //has Snake collided with wall or itself?
    bool AIsnake = 1; // AI mode

    InitialiseTerminal();
    hidecursor();

    while (!HasCollided) { //end game when snake len =  board cells
        hidecursor();
        if (Direction == 'w' || Direction == 's') {
            std::this_thread::sleep_for(std::chrono::milliseconds(115));  //Fixes : snake speeding up vertically (Illusion due to vertical gap between lines)
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(75));
        }
        //move 
        if (AIsnake) { Direction = AI(); }
        else {
            if (_kbhit()) { Direction = _getch(); }
        }
        HasCollided = mySnake.move_snake(Direction, WIDTH, HEIGHT);
        UpdateBoard();
        if (mySnake.eaten(FoodPos)) {
            GenerateFood();
            score += 10;
            mySnake.grow();
        }
    }
    setCursorPosition(1, (WIDTH - 2) / 2 - 4);
    cout << "GAME OVER !";
    setCursorPosition(2, (WIDTH - 2) / 2 - 4);
    cout << "SCORE : ";
    setCursorPosition(2, (WIDTH - 2) / 2 + 4);
    cout << score;
    setCursorPosition(HEIGHT + 1, 0);

    system("pause");
    system("pause");
    return 0;

}