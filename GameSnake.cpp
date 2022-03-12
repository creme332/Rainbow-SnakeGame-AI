#include <iostream>
#include <cstring>
#include <conio.h> 
#include "Snake.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <thread>
#include <algorithm>

#include <cstdlib>

#define HEIGHT 60
#define WIDTH 40
#define VERTICAL_WALL '|'
#define HORIZONTAL_WALL '#'
#define EMPTY ' '
#define FOOD '*'
#define SNAKEHEAD '@'
#define SNAKEBODY 'o'  //rename to snake body
//add a snake head
using namespace std;

char TerminalGrid[HEIGHT][WIDTH]; //board currently displayed on terminal
COORD grid = { WIDTH / 2, HEIGHT / 2 };
Snake mySnake(grid, 1); 
COORD FoodPos;

void GenerateFood() { //add seed
    // x [1, width - 2] and y [1, height - 2]
    FoodPos.X = (rand() % (WIDTH - 2)) + 1;
    FoodPos.Y = (rand() % (HEIGHT - 2)) + 1;
}
void setConsoleColour(unsigned short colour){
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    SetConsoleTextAttribute(hOut, colour);
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
    COORD pos = mySnake.get_pos(); //rename to SnakeHeadPosition
    //first initialise TerminalGrid 
    for (int row = 0;row < HEIGHT;row++) {
        TerminalGrid[row][0] = VERTICAL_WALL;
        for (int col = 1;col < WIDTH - 1;col++) {
            if (row == 0 || row == HEIGHT - 1)TerminalGrid[row][col] = HORIZONTAL_WALL;
            else {
                if (row == pos.Y && col == pos.X) { TerminalGrid[row][col] = SNAKEBODY; }
                else {
                    if (row == FoodPos.Y && col == FoodPos.X) {TerminalGrid[row][col] = FOOD; }
                    else { TerminalGrid[row][col] = EMPTY; }
                }
            }
        }
        TerminalGrid[row][WIDTH - 1] = VERTICAL_WALL;
    }

    //output to terminal
    for (int row = 0;row < HEIGHT;row++) {
        for (int col = 0;col < WIDTH;col++) {
            cout << TerminalGrid[row][col];
        }
        cout << endl;
    }
}

void UpdateBoard() { //create a board class later. FIRST LOOP CAN BE REMOVED.
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
    for (int row = 1;row < HEIGHT-1;row++) {
        for (int col = 1;col < WIDTH-1;col++) {
            if (newBoard[row][col] != TerminalGrid[row][col]) {
                setCursorPosition(row, col);
                std::cout << newBoard[row][col];
            }
        }
    }
    std::cout.flush();
    std::memcpy((char*)TerminalGrid, (char const*)newBoard, HEIGHT * WIDTH);
}
int main()
{
    srand(time(NULL));
    //PROBLEM : snake speeding up vertically (Illusion due to vertical gap between lines)
    int score = 0;
    InitialiseTerminal();
    char Direction = 'd';
    bool HasCollided = 0;
    hidecursor();

    setCursorPosition(0, WIDTH + 5);
    cout << "SCORE : ";
    while (!HasCollided) {
        setCursorPosition(0, WIDTH + 13);
        cout << score;
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); //lower to increase speed of snake
        if (_kbhit()) { 
            Direction = _getch();
        }
        HasCollided = mySnake.move_snake(Direction, WIDTH, HEIGHT);
        UpdateBoard();
        if (mySnake.eaten(FoodPos)) {
            GenerateFood();
            score += 10;
            mySnake.grow();
        }
 

 
    }
    //relocate cursor
    setCursorPosition(1, WIDTH + 5);
    cout << "GAME OVER   ";
    setCursorPosition(HEIGHT+10,0);
    system("pause");

}