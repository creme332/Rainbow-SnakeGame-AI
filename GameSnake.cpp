#include <iostream>
#include <cstring>
#include <conio.h> 

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define HEIGHT 50
#define WIDTH 100
#define VERTICAL_WALL '|'
#define HORIZONTAL_WALL '#'
#define EMPTY ' '
#define PIECE '0' 
using namespace std;

char prevBoard[HEIGHT][WIDTH]; //board currently displayed on terminal
int x = 4, y = 2; //current position. x = col number. y = row number

void setConsoleColour(unsigned short colour)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    SetConsoleTextAttribute(hOut, colour);
}
void setCursorPosition(const int row, const int col)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = { (SHORT)col, (SHORT)row };
    SetConsoleCursorPosition(hOut, coord);
}

void DisplayBoard() {

    //save to prevBoard 
    for (int row = 0;row < HEIGHT;row++) {
        prevBoard[row][0] = VERTICAL_WALL;
        for (int col = 1;col < WIDTH - 1;col++) {
            if (row == 0 || row == HEIGHT - 1)prevBoard[row][col] = HORIZONTAL_WALL;
            else {
                if (row == y && col == x) { prevBoard[row][col] = PIECE; }
                else {
                    prevBoard[row][col] = EMPTY;
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
void UpdateBoard() {
    //create a new board
    char newBoard[HEIGHT][WIDTH];
    for (int row = 0;row < HEIGHT;row++) {
        newBoard[row][0]=VERTICAL_WALL;
        for (int col = 1;col < WIDTH - 1;col++) {
            if (row == 0 || row == HEIGHT - 1)newBoard[row][col] = HORIZONTAL_WALL;
            else {
                if (row == y && col == x) {newBoard[row][col] = PIECE;}
                else {
                    newBoard[row][col] = EMPTY;
                }
            }
        }
        newBoard[row][WIDTH - 1] = VERTICAL_WALL;
    }
    //compare with previous board currently on screen and add changes directly to terminal
    for (int row = 0;row < HEIGHT;row++) {
        for (int col = 0;col < WIDTH;col++) {
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
    DisplayBoard();
    while (true) {
        if (_kbhit()) {
            switch (_getch()) {
            case 'w': y--;break;
            case 'a': x--;break;
            case 's': y++;break;
            case 'd': x++;break;
            }
        }
        UpdateBoard();
    }
    system("pause");

}