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
COORD MiddleGridPosition = { HEIGHT / 2, WIDTH / 2 };
COORD FoodPos; //position of food on board. FoodPos.X : row number. FoodPos.Y=col number
Snake mySnake(MiddleGridPosition, 5, WIDTH, HEIGHT); // initial position, growth rate, grid width, grid height
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

    //spawn food in a position not occupied by snake
    while (!mySnake.NotSnakeBody(FoodPos.Y, FoodPos.X)) { 
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

int main() {
    
    srand(time(NULL)); //seed for random function
    int score = 0;
    char Direction = RIGHT; //initially move right
    bool HasCollided = 0; //has Snake collided with wall or itself?
    bool AIMode = 1; // AI mode

    InitialiseTerminal();
    hidecursor();
    while (!HasCollided) {
        hidecursor();

        //if (Direction == UP || Direction == DOWN) {std::this_thread::sleep_for(std::chrono::milliseconds(40));}
        //else {std::this_thread::sleep_for(std::chrono::milliseconds(10));}

        //move 
        if (AIMode) { 
            // Direction = mySnake.AI_Hamilton();
             Direction = mySnake.AI_BFS(FoodPos);
        }
        else {
            if (_kbhit()) { Direction = _getch(); //HasCollided = mySnake.move_snake(Direction);UpdateBoard();
            }
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