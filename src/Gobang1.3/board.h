// board.h
#pragma once
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <stack>

#define SIZE 15
#define BOARD_SIZE (SIZE + 1)
#define GRID_SIZE 40
#define OFFSET 30  
#define LINE_WIDTH 2
#define SIDEBAR_WIDTH 200

extern int board[BOARD_SIZE][BOARD_SIZE];
extern double weights[BOARD_SIZE][BOARD_SIZE];
extern int key_x, key_y;
extern int role;
extern bool menuOpen;
extern std::stack<std::pair<int, int>> history;
extern int situation[36][4];
extern int situation2[18][4];

extern int baseScoreTable[36];
extern int bonusScoreTable[36];
extern int baseScoreTable2[18];
extern int bonusScoreTable2[18];

void init_board(void);
void precompute_weights();
void show_board(void);
std::pair<bool, std::pair<int, int>> check_key(int pawn_num);
void draw_sidebar(void);
void handleMenuSelection();
void undo(int steps);

