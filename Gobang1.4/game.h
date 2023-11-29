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
int getChessType(int x, int y, int dx, int dy);
int evaluate_position_player(int x, int y);
int evaluate_position_computer(int x, int y);
int evaluate();
void greedyMove();
int minimax(int depth, int alpha, int beta, bool isMaximizingPlayer, int centerX, int centerY);

void terminateGame();
void initializeGame();
void createGameButtons();


