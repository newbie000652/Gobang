// game.cpp
#include "game.h"
#include "board.h"
#include "input.h"

enum GameMode { HUMAN_VS_HUMAN, HUMAN_VS_COMPUTER };
GameMode currentGameMode = HUMAN_VS_HUMAN;

void setGameMode(GameMode mode) {
	currentGameMode = mode;
}

GameMode getGameMode() {
	return currentGameMode;
}

void showResult(const TCHAR* resultText) {
	show_board();
	draw_sidebar();
	setfillcolor(RED);
	solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, 20, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, 70);
	settextstyle(32, 0, _T("宋体"));
	settextcolor(GREEN);
	outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 30, 30, resultText);
	//_getch();     // 这里注释掉是为了游戏结束后显示菜单
	//closegraph();
}


void startDoublePlayerBattle() {
	setGameMode(HUMAN_VS_HUMAN);
	initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
	init_board();
	while (1) {
		show_board();
		draw_sidebar();
		get_key();
		if (isGameOver()) {
			break;
		}
		role = -role;
	}
}


bool isBoardFull() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == 0) {
				return false;
			}
		}
	}
	return true;
}

bool isGameOver() {
	std::pair<bool, std::pair<int, int>> result = check_key(5);
	if (result.first) {
		if (roleReverse == false) {
			showResult(role == 1 ? _T("黑子胜利!") : _T("白子胜利!")); 
		}
		else {
			showResult(role == -1 ? _T("黑子胜利!") : _T("白子胜利!"));
		}
		handleMenuSelection();
		return true;
	}
	if (isBoardFull()) {
		showResult(_T("平局！"));
		handleMenuSelection();
		return true;
	}
	return false;
}

void computerMove() {
	if (isBoardFull()) {
		return;
	}

	int bestScore = -INFINITY;
	std::pair<int, int> move;
	move.first = -1;
	move.second = -1;

	// 找到第一个空的位置作为初始的最佳位置
	int center = BOARD_SIZE / 2;
	for (int layer = 0; layer <= center; layer++) {
		for (int i = center - layer; i <= center + layer; i++) {
			for (int j = center - layer; j <= center + layer; j++) {
				if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE && board[i][j] == 0) {
					move = std::make_pair(i, j);
					bestScore = evaluate_position_computer(i, j);
					break;
				}
			}
			if (move.first != -1 && move.second != -1) {
				break;
			}
		}
		if (move.first != -1 && move.second != -1) {
			break;
		}
	}

	for (int layer = 0; layer <= center; layer++) {
		for (int i = center - layer; i <= center + layer; i++) {
			for (int j = center - layer; j <= center + layer; j++) {
				if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE && board[i][j] == 0) {
					// 尝试在这里落子
					board[i][j] = -1;
					int score = minimax(2, -INFINITY, INFINITY, false, i, j); //evaluate_position_computer(i, j);
					// 撤销尝试
					board[i][j] = 0;
					if (score > bestScore) {
						bestScore = score;
						move = std::make_pair(i, j);
					}
				}
			}
		}
	}

	// 在最优的位置落子
	board[move.first][move.second] = -1;
	key_x = move.first;
	key_y = move.second;
	history.push(move);
}

int getChessType(int x, int y, int dx, int dy) {
	int position[5];
	for (int i = 0; i < 5; i++) {
		if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
			position[i] = board[x][y];
		}
		else {
			position[i] = 0;
		}
		x += dx;
		y += dy;
	}
	for (int i = 0; i < 9; i++) {
		if (std::equal(position, position + 5, situation[i])) {
			return i;
		}
	}
	return -1;
}



double position_weight(int x, int y) {
	return weights[x][y];
}

int evaluate_position_player(int x, int y) {
	int score = 0;
	// 遍历这个位置的八个方向
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			if (dx != 0 || dy != 0) {
				int type;
				if (roleReverse == false) {
					type = getChessType(x, y, dx, dy);
				}
				else if (roleReverse == true) {
					role = -role;
					type = getChessType(x, y, dx, dy);
					role = -role;
				}
				
				score += ScoreTablePlayer[type] * position_weight(x, y);
			}
		}
	}
	return score;
}

int evaluate_position_computer(int x, int y) {
	int score = 0;
	// 遍历这个位置的八个方向
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			if (dx != 0 || dy != 0) {
				// 获取这个方向上的棋型
				int type = getChessType(x, y, dx, dy);
				// 根据棋型来计算分数
				score += ScoreTableComputer[type] * position_weight(x, y);
			}
		}
	}
	return score;
}

int evaluate() {
	int score = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == role && roleReverse == false) {
				score -= evaluate_position_player(i, j);
			}
			else if (board[i][j] == -role && roleReverse == false) {
				score += evaluate_position_computer(i, j);
			}
			else if (board[i][j] == role && roleReverse == true) {
				score += evaluate_position_computer(i, j);
			}
			else if (board[i][j] == -role && roleReverse == true) {
				score -= evaluate_position_player(i, j);
			}
		}
	}
	return score;
}

void greedyMove() {
	if (isBoardFull()) {
		return;
	}

	int bestScore = -INFINITY;
	std::pair<int, int> move;
	move.first = -1;
	move.second = -1;

	// 找到第一个空的位置作为初始的最佳位置
	int center = BOARD_SIZE / 2;
	for (int layer = 0; layer <= center; layer++) {
		for (int i = center - layer; i <= center + layer; i++) {
			for (int j = center - layer; j <= center + layer; j++) {
				if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE && board[i][j] == 0) {
					move = std::make_pair(i, j);
					bestScore = evaluate_position_computer(i, j); // 计算初始位置的分数
					break;
				}
			}
			if (move.first != -1 && move.second != -1) {
				break;
			}
		}
		if (move.first != -1 && move.second != -1) {
			break;
		}
	}

	for (int layer = 0; layer <= center; layer++) {
		for (int i = center - layer; i <= center + layer; i++) {
			for (int j = center - layer; j <= center + layer; j++) {
				if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE && board[i][j] == 0) {
					// 尝试在这里落子
					board[i][j] = -1;
					int score = evaluate_position_computer(i, j);
					// 撤销尝试
					board[i][j] = 0;
					if (score > bestScore) {
						bestScore = score;
						move = std::make_pair(i, j);
					}
				}
			}
		}
	}

	// 在最优的位置落子
	board[move.first][move.second] = -1;
	key_x = move.first;
	key_y = move.second;
	history.push(move);
}


int minimax(int depth, int alpha, int beta, bool isMaximizingPlayer, int centerX, int centerY) {
	if (depth == 0) {
		return evaluate();
	}

	int startRow = max(0, centerX - 2);
	int endRow = min(BOARD_SIZE - 1, centerX + 2);
	int startCol = max(0, centerY - 2);
	int endCol = min(BOARD_SIZE - 1, centerY + 2);

	if (isMaximizingPlayer) {
		int maxEval = -INFINITY;
		for (int i = startRow; i <= endRow; i++) {
			for (int j = startCol; j <= endCol; j++) {
				if (board[i][j] == 0) {
					// 尝试在这里落子
					board[i][j] = -1;
					int eval = minimax(depth - 1, alpha, beta, false, i, j);
					// 撤销尝试
					board[i][j] = 0;
					maxEval = max(maxEval, eval);
					alpha = max(alpha, eval);
					if (beta <= alpha) {
						return maxEval;
					}
				}
			}
		}
		return maxEval;
	}
	else {
		int minEval = INFINITY;
		for (int i = startRow; i <= endRow; i++) {
			for (int j = startCol; j <= endCol; j++) {
				if (board[i][j] == 0) {
					// 尝试在这里落子
					board[i][j] = 1;
					int eval = minimax(depth - 1, alpha, beta, true, i, j);
					// 撤销尝试
					board[i][j] = 0;
					minEval = min(minEval, eval);
					beta = min(beta, eval);
					if (beta <= alpha) {
						return minEval;
					}
				}
			}
		}
		return minEval;
	}
}


void startSinglePlayerBattle() {
	setGameMode(HUMAN_VS_COMPUTER);
	initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
	init_board();
	precompute_weights();
	while (1) {
		show_board();
		draw_sidebar();
		if (role == 1) {
			// 玩家落子
			get_key();
		}
		else {
			// 电脑落子，两种算法，选择一种即可
			computerMove();
			//greedyMove();
		}
		if (isGameOver()) {
			break;
		}
		role = -role;
	}
}


void terminateGame() {
	/* Code for exiting the game */
	closegraph();
	exit(0);
}

void initializeGame() {

	initgraph(640, 480);
	setbkcolor(YELLOW);
	cleardevice();

	createGameButtons();
	role = 1;

	while (1)
	{
		ExMessage msg;

		if (peekmessage(&msg, PM_REMOVE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				processMouseClick(msg);
			}
		}

		flushmessage();

		if (_kbhit())
		{
			processKeyboardInput();
		}
	}
}

void createGameButtons() {
	// Draw buttons
	setfillcolor(LIGHTBLUE);
	fillroundrect(220, 150, 420, 200, 20, 20);
	fillroundrect(220, 250, 420, 300, 20, 20);
	fillroundrect(220, 350, 420, 400, 20, 20);

	// Add text to the button
	settextcolor(BLACK);
	settextstyle(30, 0, _T("黑体"));
	outtextxy(260, 160, _T("双人对战"));
	outtextxy(260, 260, _T("人机对战"));
	outtextxy(260, 360, _T("退出游戏"));
}