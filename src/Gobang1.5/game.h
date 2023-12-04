// game.h
#pragma once
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <limits>
#include <cmath>
#include <stack>
#include <queue>

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
int small_assess(int my_color, int opponent_color, int i, int j);
void greedyMove();
std::pair<int, std::pair<int, int>> minimax(int depth, int alpha, int beta, bool isMaximizingPlayer);

void terminateGame();
void initializeGame();
void createGameButtons();


