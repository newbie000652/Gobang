// game.cpp
#include "game.h"
#include "board.h"
#include "input.h"

#define FIVE 90000 //连五
#define HUO_FOUR 40000 //活四
#define CHONG_FOUR 2000 //冲四
#define HUO_THREE 3000 //活三
#define MIAN_THREE 80 //眠三
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
	settextcolor(YELLOW);
	outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 30, 30, resultText);
	//_getch();     // 这里注释掉是为了游戏结束后显示菜单
}

void startDoublePlayerBattle() {
	setGameMode(HUMAN_VS_HUMAN);
	initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
	init_board();
	piece_count = 0;  // 重置棋子数量
	while (1) {
		show_board();
		draw_sidebar();
		get_key();
		piece_count++;
		if (isGameOver()) {
			break;
		}
		role = -role;
	}
}

bool isBoardFull() {
	return piece_count >= BOARD_SIZE * BOARD_SIZE;
}

bool isGameOver() {
	bool result = check_key(5, key_x, key_y);
	if (result) {
		if (playerColor == 1 && computerColor == -1) {
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

std::vector<std::pair<int, std::pair<int, int>>> heuristicEvaluation(int my_color, int opponent_color, int pos_x, int pos_y) {
	// 创建一个 multimap 来存储评估值和对应的位置
	std::multimap<int, std::pair<int, int>, std::greater<int>> mm;

	std::queue<std::pair<int, int>> q;
	q.push(std::make_pair(pos_x, pos_y));

	std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));

	while (!q.empty()) {
		std::pair<int, int> cur = q.front();
		q.pop();

		int x = cur.first;
		int y = cur.second;

		if (!visited[x][y] && board[x][y] == 0) {
			board[x][y] = my_color;
			int score = assess(my_color, opponent_color, x, y) + assess(opponent_color, my_color, x, y);
			board[x][y] = 0;

			mm.insert(std::make_pair(score, std::make_pair(x, y)));
		}

		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (dx == 0 && dy == 0) {
					continue;
				}
				if (x + dx >= 0 && x + dx < BOARD_SIZE && y + dy >= 0 && y + dy < BOARD_SIZE && !visited[x + dx][y + dy]) {
					q.push(std::make_pair(x + dx, y + dy));
				}
			}
		}

		visited[x][y] = true;
	}

	// 将 multimap 中的元素转移到一个 vector 中
	std::vector<std::pair<int, std::pair<int, int>>> bestMoves;
	for (const auto& pair : mm) {
		bestMoves.push_back(pair);
		if (bestMoves.size() >= 16) {
			break;
		}
	}

	return bestMoves;
}
std::pair<int, std::pair<int, int>> minimax2(int my_color, int opponent_color, int pos_x, int pos_y, int depth, int alpha, int beta, bool isMaximizingPlayer, int maxDepth) {
	if (depth == maxDepth || check_key(5, pos_x, pos_y) || isBoardFull()) {
		return std::make_pair((isMaximizingPlayer ? 1 : -1) * (assess(my_color, opponent_color, pos_x, pos_y) - assess(opponent_color, my_color, pos_x, pos_y)), std::make_pair(pos_x, pos_y));
	}

	int bestScore = isMaximizingPlayer ? (std::numeric_limits<int>::min)() : (std::numeric_limits<int>::max)();
	std::pair<int, int> move = std::make_pair(-1, -1);

	// 获取16个最优点
	auto bestMoves = heuristicEvaluation(isMaximizingPlayer ? -1 : 1, isMaximizingPlayer ? 1 : -1, pos_x, pos_y);

	// 预搜索：对每个候选点进行一次浅层次的搜索，获取评估值
	std::map<int, std::pair<int, int>, std::greater<int>> preScores;
	for (const auto& bestMove : bestMoves) {
		int x = bestMove.second.first;
		int y = bestMove.second.second;

		board[x][y] = isMaximizingPlayer ? -1 : 1;
		int score = assess(my_color, opponent_color, x, y) + assess(opponent_color, my_color, x, y);
		board[x][y] = 0;

		preScores.insert(std::make_pair(score, std::make_pair(x, y)));
	}

	// 对排序后的候选点进行深度搜索
	for (const auto& preScore : preScores) {
		int x = preScore.second.first;
		int y = preScore.second.second;

		board[x][y] = isMaximizingPlayer ? -1 : 1;

		// 检查五子连珠, 这一段如果去掉会慢很多（好像是奇奇怪怪的bug）
		bool check_result = check_key(isMaximizingPlayer ? -1 : 1, x, y);
		if (check_result) {
			board[x][y] = 0;
			move.first = x;
			move.second = y;
			return std::make_pair(isMaximizingPlayer ? 990000 : -990000, move);
		}

		int score = minimax2(opponent_color, my_color, x, y, depth + 1, alpha, beta, !isMaximizingPlayer, maxDepth).first;
		board[x][y] = 0;

		if (isMaximizingPlayer && score > bestScore) {
			bestScore = score;
			move = preScore.second;
			alpha = std::fmax(alpha, score);
		}
		else if (!isMaximizingPlayer && score < bestScore) {
			bestScore = score;
			move = preScore.second;
			beta = std::fmin(beta, score);
		}

		if (beta <= alpha) {
			break;
		}
	}

	return std::make_pair(bestScore, move);
}

std::pair<int, int> greedyMove() {
	if (isBoardFull()) {
		return std::make_pair(-1, -1);
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

		// 广搜
		std::queue<std::pair<int, int>> q;
		q.push(std::make_pair(centerX, centerY));

		std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));

		while (!q.empty()) {
			std::pair<int, int> cur = q.front();
			q.pop();

			int x = cur.first;
			int y = cur.second;

			bool isContinue = false;
			if (std::abs(x - centerX) > 7 || std::abs(y - centerY) > 7) {
				isContinue = true;
			}

			if (!isContinue && !visited[x][y] && board[x][y] == 0) {
				board[x][y] = -1;
				int score = assess(role, -role, x, y) + assess(-role, role, x, y);
				board[x][y] = 0;
				if (score > bestScore) {
					bestScore = score;
					move = std::make_pair(x, y);
				}
			}

			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if (i == 0 && j == 0) {
						continue;
					}
					if (x + i >= 0 && x + i < BOARD_SIZE && y + j >= 0 && y + j < BOARD_SIZE && !visited[x + i][y + j]) {
						q.push(std::make_pair(x + i, y + j));
					}
				}
			}

			visited[x][y] = true;
		}
	}

	return move;
}

int assess(int my_color, int opponent_color, int i, int j)
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
	for (int k = 0; k < 8; k++) {
		int count = 1; 
		int empty = 0; 
		int block = 0; 
		int x = i + direction[k][0]; //沿着方向移动
		int y = j + direction[k][1];
		while (board[x][y] == my_color) {
			count++;
			x += direction[k][0];
			y += direction[k][1];
		}
		if (board[x][y] == 0) {
			empty++;
		}
		else {
			block++;
		}
		x = i - direction[k][0]; //反向移动
		y = j - direction[k][1];
		while (board[x][y] == my_color) {
			count++;
			x -= direction[k][0];
			y -= direction[k][1];
		}
		if (board[x][y] == 0) {
			empty++;
		}
		else {
			block++;
		}
		//根据连子数、空位数和堵住数判断棋形，并给分
		if (count >= 5) { //连五
			if (board[i][j] == 1) { // 玩家
				score += FIVE;
			}
			else {
				score += FIVE * 1.2;	// 电脑的连五分更高
			}
		}
		else if (count == 4) { //连四
			if (empty == 2) { //活四
				if (board[i][j] == 1) {
					score += HUO_FOUR * 1.0;
				}
				else {
					score += HUO_FOUR;
				}
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
					if (board[i][j] == 1) { // 玩家
						score += HUO_THREE * 1.2;
					}
					else {
						score += HUO_THREE;
					}
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

// 有莫名 bug 的 minimax 函数
std::pair<int, std::pair<int, int>> minimax(int role1, int role2, int pos_x, int pos_y, int depth, int alpha, int beta, bool isMaximizingPlayer, int maxDepth) {
	if (depth == maxDepth || isBoardFull()) {
		return std::make_pair((isMaximizingPlayer ? 1 : -1) * ((assess(role1, role2, pos_x, pos_y) - assess(role2, role1, pos_x, pos_y))), std::make_pair(pos_x, pos_y));
	}

	int centerX = pos_x;
	int centerY = pos_y;

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
			int score = minimax(role2, role1, x, y, depth + 1, alpha, beta, !isMaximizingPlayer, maxDepth).first;
			board[x][y] = 0;

			if (isMaximizingPlayer && score > bestScore) {
				bestScore = score;
				move = cur;
				alpha = std::fmax(alpha, score);
				noImprovementCounter = 0; 
			}
			else if (!isMaximizingPlayer && score < bestScore) {
				bestScore = score;
				move = cur;
				beta = std::fmin(beta, score);
				noImprovementCounter = 0; 
			}
			else {
				noImprovementCounter++; 
			}

			if (beta < alpha || noImprovementCounter >= 18) { // 如果连续18次没有找到更优的点，提前结束循环
				break;
			}
		}

		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				if (i == 0 && j == 0) {
					continue;
				}
				if (x + i >= 0 && x + i < BOARD_SIZE && y + j >= 0 && y + j < BOARD_SIZE && !visited[x + i][y + j]) {
					q.push(std::make_pair(x + i, y + j));
				}
			}
		}

		visited[x][y] = true;
	}

	return std::make_pair(bestScore, move);
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
		int maxDepth = 7;
		std::pair<int, std::pair<int, int>> result;
		if (history.size() < 4) {
			result.second = greedyMove();
		}
		else {
			//result = minimax(role, -role, key_x, key_y, 0, (std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), false, maxDepth);
			result = minimax2(role, -role, key_x, key_y, 0, (std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), true, maxDepth);
		}

		// 落子
		board[result.second.first][result.second.second] = -1;
		key_x = result.second.first;
		key_y = result.second.second;
		history.push(result.second);
	}
}

void startSinglePlayerBattle() {
	setGameMode(HUMAN_VS_COMPUTER);
	initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
	init_board();
	piece_count = 0;  // 记录已经落子的数目
	while (1) {
		show_board();
		draw_sidebar();
		if (role == 1) {
			get_key();
			piece_count++;  // 增加落子数目
		}
		else {
			computerMove();
			piece_count++;
		}
		if (isGameOver()) {
			break;
		}
		role = -role;
	}
}

void terminateGame() {
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
	// 按钮背景
	setfillcolor(LIGHTBLUE);
	fillroundrect(220, 150, 420, 200, 20, 20);
	fillroundrect(220, 250, 420, 300, 20, 20);
	fillroundrect(220, 350, 420, 400, 20, 20);

	// 大标题
	settextcolor(BLACK);
	settextstyle(60, 0, _T("宋体"));
	outtextxy(230, 50, _T("五子棋"));

	// 小按钮字体
	settextcolor(BLACK);
	settextstyle(30, 0, _T("黑体"));
	setbkmode(TRANSPARENT);
	outtextxy(260, 160, _T("双人对战"));
	outtextxy(260, 260, _T("人机对战"));
	outtextxy(260, 360, _T("退出游戏"));
}