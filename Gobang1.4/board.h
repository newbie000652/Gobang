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

extern const int situation[9][5];
extern const int ScoreTablePlayer[9];
extern const int ScoreTableComputer[9];

void init_board(void);
void precompute_weights();
void show_board(void);
std::pair<bool, std::pair<int, int>> check_key(int pawn_num);
void draw_sidebar(void);
void handleMenuSelection();
void undo(int steps);

