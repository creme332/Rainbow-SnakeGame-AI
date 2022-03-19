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
#define WIDTH 50
#define VERTICAL_WALL '|'
#define HORIZONTAL_WALL '#'
#define EMPTY ' '
#define FOOD '*'
#define SNAKEHEAD '@'
#define SNAKEBODY 'o'  
#define LEFT 'a'
#define RIGHT 'd'
#define UP 'w'
#define DOWN 's'
using namespace std;

char TerminalGrid[HEIGHT][WIDTH]; //board currently displayed on terminal
COORD MiddleGrid = { HEIGHT / 2, WIDTH / 2 };
COORD FoodPos; //position of food on board. FoodPos.X : row number. FoodPos.Y=col number

Snake mySnake(MiddleGrid, WIDTH, HEIGHT);
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

void GenerateFood() {
    // col in [1, width - 2] and row in [1, height - 2]
    FoodPos.Y = (rand() % (WIDTH - 2)) + 1; //col
    FoodPos.X = (rand() % (HEIGHT - 2)) + 1; //row

    while (!mySnake.NotSnakeBody(FoodPos.Y, FoodPos.X)) { //spawn food in a position not occupied by snake
        FoodPos.Y = (rand() % (WIDTH - 2)) + 1;
        FoodPos.X = (rand() % (HEIGHT - 2)) + 1;
    }

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
                if (row == SnakeHeadPosition.X && col == SnakeHeadPosition.Y) { TerminalGrid[row][col] = SNAKEHEAD; }
                else {
                    if (row == FoodPos.X && col == FoodPos.Y) { TerminalGrid[row][col] = FOOD; }
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
    //create a new board
    char newBoard[HEIGHT][WIDTH];

    for (int row = 0;row < HEIGHT;row++) {
        newBoard[row][0] = VERTICAL_WALL;
        for (int col = 1;col < WIDTH - 1;col++) {
            if (row == 0 || row == HEIGHT - 1)newBoard[row][col] = HORIZONTAL_WALL;
            else {
                if (SnakeHeadPosition.X == row && SnakeHeadPosition.Y == col) { newBoard[row][col] = SNAKEHEAD; }
                else {
                    if (row == FoodPos.X && col == FoodPos.Y) { newBoard[row][col] = FOOD; }
                    else {
                        bool body = 0;
                        for (auto i : snakebody) {
                            if (i.X == row && i.Y == col) body = 1; newBoard[row][col] = SNAKEBODY;
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

std::stack <std::pair<int, int>> maxPath() {// using brute force
    int startcol = mySnake.get_pos().Y;
    int startrow = mySnake.get_pos().X;
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
        setCursorPosition(currentcoord.first, currentcoord.second);
        cout << Color(SNAKEBODY);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

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

                if (x > 0 && x < HEIGHT - 1 && y > 0
                    && y < WIDTH - 1 && parentnode.count({ x,y }) == 0 //unvisited
                    && mySnake.NotSnakeBody(x, y)) {
                    NextNode.push({ x,y });
                    if (currentdistance + 1 > distance[{x, y}]) {
                        distance[{x, y}] = currentdistance + 1;
                        parentnode[{x, y}] = { currentcoord.first, currentcoord.second };
                    }
                }

            }
        }

    }
    //if path does not exist to food ?????
    return longestpath;

}

std::stack <std::pair<int, int>> longestpath; //stores nodes along longest path in (row,col) format

void rec(int startrow, int startcol, int finishrow, int finishcol, int currentdistance) {
    static std::map < std::pair<int, int>, bool > AlreadyVisited = { {{startrow, startcol},1} };
    static std::map <std::pair<int, int>, std::pair<int, int>> parentnode = { {{startrow, startcol}, {-1,-1}} }; // {node, parent node}
    static int maxdistance = -1;
    std::vector <int> dx = { 1,0,-1,0 }; // translation horizontally
    std::vector <int> dy = { 0,1,0,-1 }; //translation vertically
    int x, y;

    if (startrow == finishrow && startcol == finishcol) {
        if (currentdistance > maxdistance) {
            maxdistance = currentdistance;
            longestpath.empty();
            int x0, y0;
            x = startrow;
            y = startcol;
            while (x != -1) { // while start node not reached, 
                longestpath.push({ x, y });
                x0 = x; y0 = y;
                x = parentnode[{x0, y0}].first;
                y = parentnode[{x0, y0}].second;
            }
        }
        return;
    }

    for (int i = 0; i < 4;i++) {
        x = startrow + dx[i]; //row 
        y = startcol + dy[i]; //col

        if (x > 0 && x < HEIGHT - 1 && y > 0 && y < WIDTH - 1 && AlreadyVisited[{x,y}] == 0) {
            AlreadyVisited[{x, y}] = 1;
            parentnode[{x, y}] = { startrow, startcol };
            rec(x, y, finishrow, finishcol, currentdistance + 1);
            AlreadyVisited[{x, y}] = 0;
        }
    }
}
char AI() { //bfs
    //add to snake class
    COORD SnakeHeadPosition = mySnake.get_pos();
    int startcol = SnakeHeadPosition.Y;
    int startrow = SnakeHeadPosition.X;
    int finishcol = FoodPos.Y;
    int finishrow = FoodPos.X;
    std::vector <int> dx = { 1,0,-1,0 }; // translation horizontally
    std::vector <int> dy = { 0,1,0,-1 }; //translation vertically

    char D1 = mySnake.minPath(FoodPos);
    int x, y;
    if (D1 == ERROR) { //if no path exist, move to a any free position
        for (int i = 0;i < 4;i++) {
            x = startrow + dx[i]; // new row 
            y = startcol + dy[i]; // new col
            if (x > 0 && x < HEIGHT - 1 && y > 0 //validation
                && y < WIDTH - 1 && mySnake.NotSnakeBody(x, y)) {
                //free position found
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
    }
    return D1;
}

int main() {
    
    srand(time(NULL)); //seed for random function
    int score = 0;
    char Direction = RIGHT; //initially move right
    bool HasCollided = 0; //has Snake collided with wall or itself?
    bool AIMode = 1; // AI mode

    InitialiseTerminal();
    hidecursor();

    while (!HasCollided) { //end game when snake len =  board cells
        hidecursor();
        if (Direction == UP || Direction == DOWN) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));  //Fixes : snake speeding up vertically (Illusion due to vertical gap between lines)
        }
        else {std::this_thread::sleep_for(std::chrono::milliseconds(10));}

        //move 
        if (AIMode) { Direction = mySnake.AI(FoodPos); }
        else {
            if (_kbhit()) { Direction = _getch(); HasCollided = mySnake.move_snake(Direction);UpdateBoard(); }
        }
        HasCollided = mySnake.move_snake(Direction);
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