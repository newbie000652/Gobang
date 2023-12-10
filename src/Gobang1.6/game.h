// game.h
#pragma once
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <limits>
#include <cmath>
#include <stack>
#include <queue>
#include <map>

extern enum GameMode;
extern GameMode currentGameMode;

void startDoublePlayerBattle();
void startSinglePlayerBattle();
void setGameMode(GameMode mode);
GameMode getGameMode();
void showResult(const TCHAR* resultText);

bool isBoardFull();
bool isGameOver();
std::vector<std::pair<int, std::pair<int, int>>> heuristicEvaluation(int my_color, int opponent_color, int pos_x, int pos_y);

void computerMove();
int assess(int my_color, int opponent_color, int i, int j);
int assess_for_greedy(int my_color, int opponent_color, int i, int j);
std::pair<int, int> greedyMove();
std::pair<int, int> greedyMove2();
std::pair<int, int> greedyMove3(int my_color, int opponent_color, int pos_x, int pos_y);
bool isNeighbor(int x, int y);
std::pair<int, std::pair<int, int>> minimax(int depth, int alpha, int beta, bool isMaximizingPlayer, int maxDepth);
std::pair<int, std::pair<int, int>> minimax2(int my_color, int opponent_color, int depth, int alpha, int beta, bool isMaximizingPlayer, int maxDepth, int pos_x, int pos_y);

void terminateGame();
void initializeGame();
void createGameButtons();


