#include <iostream>
#include <cstring>
#include <conio.h> 
#include <string>
#include "Snake.h"
#include <Windows.h>
#include <thread>
#include <algorithm>
#include <cstdlib>

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

string Color(char c) { // add colors to terminal
   int color[] = { 51};
    if(c==SNAKEHEAD) return  "\033[48;5;10m\033[38;5;232m \033[0m"; //green block for head
    if (c == SNAKEBODY) {
        int k = rand() % (sizeof(color) / sizeof(color[0])); //pick a random color for snake body
        return  "\033[48;5;" + to_string(color[k])+ "m\033[38;5;232m \033[0m"; 
    } 
    if(c==HORIZONTAL_WALL|| c==VERTICAL_WALL)return  "\033[48;5;15m\033[38;5;232m \033[0m";
    if (c == FOOD) return  "\033[48;5;196m\033[38;5;232m \033[0m"; //red block for food
    string output = ""; output += c;
    return output;
}
void GenerateFood() { 
    vector <COORD> snakebody = mySnake.get_body();
    // x [1, width - 2] and y [1, height - 2]
    bool NotFree = 1; //is position not free?
    while (NotFree) {
        NotFree = 0;
        FoodPos.X = (rand() % (WIDTH - 2)) + 1;
        FoodPos.Y = (rand() % (HEIGHT - 2)) + 1;
        for (int i = 0;i < snakebody.size();i++) {
            if (snakebody[i].X == FoodPos.X && snakebody[i].Y == FoodPos.Y) {
                NotFree = 1; break;
            }
        }
        if (NotFree == 0)break;
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
             cout << Color(TerminalGrid[row][col]);
        }
        cout << endl;
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
            if (newBoard[row][col] != TerminalGrid[row][col] || newBoard[row][col] == VERTICAL_WALL || newBoard[row][col] == HORIZONTAL_WALL || newBoard[row][col] == FOOD) { //for when terminal is resized
                setCursorPosition(row, col);
                std::cout << Color(newBoard[row][col]);
            }
        }
    }
    std::cout.flush();
    std::memcpy((char*)TerminalGrid, (char const*)newBoard, HEIGHT * WIDTH);
}
int main(){
    srand(time(NULL)); //seed for random function
   
    int score = 0;
    InitialiseTerminal();
    char Direction = 'd'; //initially move right
    bool HasCollided = 0;


    while (!HasCollided) { //end game when snake len =  board cells
        hidecursor();
        if (Direction == 'w' || Direction == 's') {
            std::this_thread::sleep_for(std::chrono::milliseconds(115));  //Fixes : snake speeding up vertically (Illusion due to vertical gap between lines)
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(75));
        }
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
    setCursorPosition(1, (WIDTH-2)/2 - 4);
    cout << "GAME OVER !";
    setCursorPosition(2, (WIDTH - 2)/2 - 4);
    cout << "SCORE : ";
    setCursorPosition(2, (WIDTH - 2) / 2 + 4);
    cout << score;
    setCursorPosition(HEIGHT+1,0);
    system("pause");
    system("pause");
    return 0;

}