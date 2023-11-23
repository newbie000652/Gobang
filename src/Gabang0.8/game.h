
#pragma once
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <stdio.h>
#include <stack>

#define SIZE 15
#define BOARD_SIZE (SIZE + 1)
#define GRID_SIZE 40
#define OFFSET 30  
#define LINE_WIDTH 2
#define SIDEBAR_WIDTH 200

extern int board[BOARD_SIZE][BOARD_SIZE];
extern int key_x, key_y;
extern int role;
extern std::stack<std::pair<int, int>> history;

void startDoublePlayerBattle();
void startSinglePlayerBattle();
void terminateGame();
void initializeGame();
void createGameButtons();
void processMouseClick(ExMessage msg);
void processKeyboardInput();
void init_board(void);
void show_board(void);
void get_key(void);
bool check_key(void);
void draw_sidebar(void);
void undo(void);
