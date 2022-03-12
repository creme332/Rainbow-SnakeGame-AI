#include <iostream>
#include <cstring>
#include <conio.h> 
#include "Snake.h"
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdlib>

#define HEIGHT 50
#define WIDTH 100
#define VERTICAL_WALL '|'
#define HORIZONTAL_WALL '#'
#define EMPTY ' '
#define FOOD '@'
#define PIECE '0' 
using namespace std;

char prevBoard[HEIGHT][WIDTH]; //board currently displayed on terminal
Snake mySnake({ WIDTH / 2,HEIGHT / 2 }, 1); 
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
    COORD pos = mySnake.get_pos(); //rename to SnakePos
    //first initialise prevBoard 
    for (int row = 0;row < HEIGHT;row++) {
        prevBoard[row][0] = VERTICAL_WALL;
        for (int col = 1;col < WIDTH - 1;col++) {
            if (row == 0 || row == HEIGHT - 1)prevBoard[row][col] = HORIZONTAL_WALL;
            else {
                if (row == pos.Y && col == pos.X) { prevBoard[row][col] = PIECE; }
                else {
                    if (row == FoodPos.Y && col == FoodPos.X) {prevBoard[row][col] = FOOD; }
                    else { prevBoard[row][col] = EMPTY; }
                }
            }
        }
        prevBoard[row][WIDTH - 1] = VERTICAL_WALL;
    }

    //output to terminal
    for (int row = 0;row < HEIGHT;row++) {
        for (int col = 0;col < WIDTH;col++) {
            cout << prevBoard[row][col];
        }
        cout << endl;
    }
}
void UpdateBoard() { //create a board class later
    COORD pos = mySnake.get_pos();

    //create a new board
    char newBoard[HEIGHT][WIDTH];
    for (int row = 0;row < HEIGHT;row++) {
        newBoard[row][0]=VERTICAL_WALL;
        for (int col = 1;col < WIDTH - 1;col++) {
            if (row == 0 || row == HEIGHT - 1)newBoard[row][col] = HORIZONTAL_WALL;
            else {
                if (row == pos.Y && col == pos.X) {newBoard[row][col] = PIECE;}
                else {
                    if (row == FoodPos.Y && col == FoodPos.X) {newBoard[row][col] = FOOD; }
                    else { newBoard[row][col] = EMPTY; }
                }
            }
        }
        newBoard[row][WIDTH - 1] = VERTICAL_WALL;
    }
    //compare with previous board currently on screen and add changes directly to terminal
    //no need to check boundaries since they never change
    for (int row = 1;row < HEIGHT-1;row++) {
        for (int col = 1;col < WIDTH-1;col++) {
            if (newBoard[row][col] != prevBoard[row][col]) {
                setCursorPosition(row, col);
                std::cout << newBoard[row][col];
            }
        }
    }
    std::cout.flush();
    std::memcpy((char*)prevBoard, (char const*)newBoard, HEIGHT * WIDTH);
}
int main()
{
    InitialiseTerminal();
    while (true) {
        hidecursor();
        if (_kbhit()) {mySnake.move_snake(_getch());}
        UpdateBoard();
        if (mySnake.eaten(FoodPos)) {
            GenerateFood();
            if (FoodPos.X <1 || FoodPos.X>WIDTH - 2 || FoodPos.Y < 1 || FoodPos.Y> HEIGHT - 2 ) {
                cout << FoodPos.X<<"\n";
                cout << FoodPos.Y<<"\n";

                system("pause");
            }
            mySnake.grow();
        }
    }
    system("pause");

}