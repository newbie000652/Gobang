// game.h
#pragma once
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <cmath>
#include <stack>

extern enum GameMode;
extern GameMode currentGameMode;

void startDoublePlayerBattle();
void startSinglePlayerBattle();
void setGameMode(GameMode mode);
GameMode getGameMode();
void showResult(const TCHAR* resultText);

bool isBoardFull();
bool isGameOver();

void computerMove();
double position_weight(int x, int y);
int evaluate_position(int x, int y);
int evaluate();
void greedyMove();
int minimax(int depth, int alpha, int beta, bool isMaximizingPlayer);

// ∆¿π¿œµÕ≥
int getChessType(int x, int y, int dx, int dy);
int calculate(int* position);
int calculate2(int* position);
int mark1(int x, int y);
int mark2(int x, int y);
int mark3(int x, int y);
int mark4(int x, int y);
int mark5(int x, int y);

void terminateGame();
void initializeGame();
void createGameButtons();


