// game.h
#pragma once
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <stdio.h>
#include <cmath>
#include <stack>

void startDoublePlayerBattle();
void startSinglePlayerBattle();
void showResult(const TCHAR* resultText);

void computerMove();
bool isBoardFull();
double position_weight(int x, int y);
int evaluate_position(int x, int y);
int evaluate();
int minimax(int depth, int alpha, int beta, bool isMaximizingPlayer);

void terminateGame();
void initializeGame();
void createGameButtons();


