// game.cpp
#include "game.h"
#include "board.h"
#include "input.h"


#define FIVE 2000000 //连五
#define HUO_FOUR 2000000 //活四
#define CHONG_FOUR 3000 //冲四
#define HUO_THREE 3000 //活三
#define MIAN_THREE 100 //眠三
#define HUO_TWO 100 //活二
#define MIAN_TWO 10 //眠二
#define ONE 1 //单一


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

void greedyMove() {
	if (isBoardFull()) {
		return;
	}

	int bestScore = std::numeric_limits<int>::lowest();
	std::pair<int, int> move;

	// 如果是游戏开始，电脑先手，选择棋盘中心
	if (history.empty()) {
		move = std::make_pair(BOARD_SIZE / 2, BOARD_SIZE / 2);
	}
	else {
		// 找到最新落子的位置
		int centerX = key_x;
		int centerY = key_y;

		// 使用队列实现广度优先搜索
		std::queue<std::pair<int, int>> q;
		q.push(std::make_pair(centerX, centerY));

		// 创建一个二维布尔数组来记录每个位置是否已经被访问过
		std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));

		while (!q.empty()) {
			std::pair<int, int> cur = q.front();
			q.pop();

			int x = cur.first;
			int y = cur.second;

			if (std::abs(x - centerX) > 4 || std::abs(y - centerY) > 4) {
				continue;
			}

			if (!visited[x][y] && board[x][y] == 0) {
				// 尝试在这里落子
				board[x][y] = -1;
				int score = small_assess(role, -role, x, y) - small_assess(-role, role, x, y);
				// 撤销尝试
				board[x][y] = 0;
				if (score > bestScore) {
					bestScore = score;
					move = std::make_pair(x, y);
				}
			}

			if (x > 0 && !visited[x - 1][y]) q.push(std::make_pair(x - 1, y));
			if (x < BOARD_SIZE - 1 && !visited[x + 1][y]) q.push(std::make_pair(x + 1, y));
			if (y > 0 && !visited[x][y - 1]) q.push(std::make_pair(x, y - 1));
			if (y < BOARD_SIZE - 1 && !visited[x][y + 1]) q.push(std::make_pair(x, y + 1));

			visited[x][y] = true;
		}
	}

	// 在最优的位置落子
	board[move.first][move.second] = -1;
	key_x = move.first;
	key_y = move.second;
	history.push(move);
}

int small_assess(int my_color, int opponent_color, int i, int j)
{
	int direction[8][2] = {
	{-1, -1}, //左上
	{-1, 0}, //上
	{-1, 1}, //右上
	{0, -1}, //左
	{0, 1}, //右
	{1, -1}, //左下
	{1, 0}, //下
	{1, 1} //右下
	};

	//左上1/4棋盘的位置评分
	int weight[16][16] = {
	{100, -5, 10, 5, 5, 10, -5, 100},
	{-5, -45, 1, 1, 1, 1, -45, -5},
	{10, 1, 3, 2, 2, 3, 1, 10},
	{5, 1, 2, 1, 1, 2, 1, 5},
	{5, 1, 2, 1, 1, 2, 1, 5},
	{10, 1, 3, 2, 2, 3, 1, 10},
	{-5, -45, 1, 1, 1, 1, -45, -5},
	{100, -5, 10, 5, 5, 10, -5, 100}
	};

	int score = 0;
	score += weight[i < 8 ? i : 16 - i][j < 8 ? j : 16 - j];
	for (int k = 0; k < 8; k++) { //遍历八个方向
		int count = 1; //连子数
		int empty = 0; //空位数
		int block = 0; //堵住数
		int x = i + direction[k][0]; //沿着方向移动
		int y = j + direction[k][1];
		while (board[x][y] == my_color) { //如果是自己的棋子，连子数加一
			count++;
			x += direction[k][0];
			y += direction[k][1];
		}
		if (board[x][y] == 0) { //如果是空位，空位数加一
			empty++;
		}
		else { //如果是对手的棋子或者出界，堵住数加一
			block++;
		}
		x = i - direction[k][0]; //反向移动
		y = j - direction[k][1];
		while (board[x][y] == my_color) { //如果是自己的棋子，连子数加一
			count++;
			x -= direction[k][0];
			y -= direction[k][1];
		}
		if (board[x][y] == 0) { //如果是空位，空位数加一
			empty++;
		}
		else { //如果是对手的棋子或者出界，堵住数加一
			block++;
		}
		//根据连子数、空位数和堵住数判断棋形，并给分
		if (count == 5) { //连五
			score += FIVE;
		}
		else if (count == 4) { //连四
			if (empty == 2) { //活四
				score += HUO_FOUR;
			}
			else if (empty == 1) { //冲四
				score += CHONG_FOUR;
			}
		}
		else if (count == 3) { //连三
			if (empty == 2) { //活三
				if (block == 0) { //双活三
					score += HUO_THREE * 2;
				}
				else { //单活三
					score += HUO_THREE;
				}
			}
			else if (empty == 1) { //眠三
				score += MIAN_THREE;
			}
		}
		else if (count == 2) { //连二
			if (empty == 2) { //活二
				score += HUO_TWO;
			}
			else if (empty == 1) { //眠二
				score += MIAN_TWO;
			}
		}
		else if (count == 1)
		{
			score += ONE;
		}
	}
	return score;
}

void computerMove() {
	if (isBoardFull()) {
		return;
	}

	if (history.empty()) {
		board[BOARD_SIZE / 2][BOARD_SIZE / 2] = -1;
		key_x = BOARD_SIZE / 2;
		key_y = BOARD_SIZE / 2;
		history.push(std::make_pair(key_x, key_y));
	}
	else {

		std::pair<int, std::pair<int, int>> result = minimax(6, (std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), true);

		// 在最优的位置落子
		board[result.second.first][result.second.second] = -1;
		key_x = result.second.first;
		key_y = result.second.second;
		history.push(result.second);
	}
}

std::pair<int, std::pair<int, int>> minimax(int depth, int alpha, int beta, bool isMaximizingPlayer) {
	if (depth == 0 || isBoardFull()) {
		return std::make_pair(small_assess(role, -role, key_x, key_y) - small_assess(-role, role, key_x, key_y), std::make_pair(key_x, key_y));
	}

	int centerX = key_x;
	int centerY = key_y;

	std::queue<std::pair<int, int>> q;
	q.push(std::make_pair(centerX, centerY));

	std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));

	int bestScore = isMaximizingPlayer ? (std::numeric_limits<int>::min)() : (std::numeric_limits<int>::max)();
	std::pair<int, int> move = std::make_pair(-1, -1);

	int noImprovementCounter = 0; // 添加一个计数器来跟踪连续未找到更优点的次数

	while (!q.empty()) {
		std::pair<int, int> cur = q.front();
		q.pop();

		int x = cur.first;
		int y = cur.second;

		if (std::abs(x - centerX) > 5 || std::abs(y - centerY) > 5) {
			continue;
		}

		if (!visited[x][y] && board[x][y] == 0) {
			board[x][y] = isMaximizingPlayer ? -1 : 1;
			int score = minimax(depth - 1, alpha, beta, !isMaximizingPlayer).first;
			board[x][y] = 0;

			if (isMaximizingPlayer && score > bestScore) {
				bestScore = score;
				move = cur;
				alpha = std::fmax(alpha, score);
				noImprovementCounter = 0; // 如果找到了更优的点，重置计数器
			}
			else if (!isMaximizingPlayer && score < bestScore) {
				bestScore = score;
				move = cur;
				beta = std::fmin(beta, score);
				noImprovementCounter = 0; // 如果找到了更优的点，重置计数器
			}
			else {
				noImprovementCounter++; // 如果没有找到更优的点，增加计数器
			}

			if (beta <= alpha || noImprovementCounter >= 36) { // 如果连续36次没有找到更优的点，提前结束循环
				break;
			}
		}

		if (x > 0 && !visited[x - 1][y]) q.push(std::make_pair(x - 1, y));
		if (x < BOARD_SIZE - 1 && !visited[x + 1][y]) q.push(std::make_pair(x + 1, y));
		if (y > 0 && !visited[x][y - 1]) q.push(std::make_pair(x, y - 1));
		if (y < BOARD_SIZE - 1 && !visited[x][y + 1]) q.push(std::make_pair(x, y + 1));

		visited[x][y] = true;
	}

	return std::make_pair(bestScore, move);
}

void startSinglePlayerBattle() {
	setGameMode(HUMAN_VS_COMPUTER);
	initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
	init_board();
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