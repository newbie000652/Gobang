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
	settextcolor(GREEN);
	outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 30, 30, resultText);
	//_getch();     // 这里注释掉是为了游戏结束后显示菜单
	//closegraph();
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

std::vector<std::pair<int, std::pair<int, int>>> heuristicEvaluation(int my_color, int opponent_color, int pos_x, int pos_y) {
	// 创建一个 multimap 来存储评估值和对应的位置
	std::multimap<int, std::pair<int, int>, std::greater<int>> mm;

	// 创建一个队列用于广度优先搜索
	std::queue<std::pair<int, int>> q;
	q.push(std::make_pair(pos_x, pos_y));

	// 创建一个二维布尔数组来记录每个位置是否已经被访问过
	std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));

	while (!q.empty()) {
		std::pair<int, int> cur = q.front();
		q.pop();

		int x = cur.first;
		int y = cur.second;

		// 只对空位置进行评估
		if (!visited[x][y] && board[x][y] == 0) {
			// 尝试在这里落子
			board[x][y] = my_color;
			int score = assess(my_color, opponent_color, x, y) + assess(opponent_color, my_color, x, y);
			// 撤销尝试
			board[x][y] = 0;

			// 将评估值和对应的位置加入 multimap
			mm.insert(std::make_pair(score, std::make_pair(x, y)));
		}

		// 向周围八个方向搜索
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				// 跳过自身
				if (dx == 0 && dy == 0) {
					continue;
				}
				// 如果没有访问过，且在棋盘范围内，加入队列
				if (x + dx >= 0 && x + dx < BOARD_SIZE && y + dy >= 0 && y + dy < BOARD_SIZE && !visited[x + dx][y + dy]) {
					q.push(std::make_pair(x + dx, y + dy));
				}
			}
		}

		visited[x][y] = true;
	}

	// 将 multimap 中的元素转移到一个向量中
	std::vector<std::pair<int, std::pair<int, int>>> bestMoves;
	for (const auto& pair : mm) {
		bestMoves.push_back(pair);
		// 如果向量的大小超过16，就停止添加
		if (bestMoves.size() >= 16) {
			break;
		}
	}

	return bestMoves;
}
std::pair<int, std::pair<int, int>> minimax2(int my_color, int opponent_color, int depth, int alpha, int beta, bool isMaximizingPlayer, int maxDepth, int pos_x, int pos_y) {
	if (depth == maxDepth || isBoardFull()) {
		return std::make_pair(assess(my_color, opponent_color, pos_x, pos_y) - assess(opponent_color, my_color, pos_x, pos_y), std::make_pair(pos_x, pos_y));
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

		board[x][y] = isMaximizingPlayer ? my_color : opponent_color;
		int score = assess(my_color, opponent_color, x, y) - assess(opponent_color, my_color, x, y);
		board[x][y] = 0;

		preScores.insert(std::make_pair(score, std::make_pair(x, y)));
	}

	// 对排序后的候选点进行深度搜索
	for (const auto& preScore : preScores) {
		int x = preScore.second.first;
		int y = preScore.second.second;

		board[x][y] = isMaximizingPlayer ? my_color : opponent_color;

		// 检查五子连珠
		auto check_result = check_key(isMaximizingPlayer ? my_color : opponent_color);
		if (check_result.first) {
			board[x][y] = 0;
			move.first = x;
			move.second = y;
			return std::make_pair(isMaximizingPlayer ? 8000000 : -8000000, move);
		}

		int score = minimax2(opponent_color, my_color, depth + 1, alpha, beta, !isMaximizingPlayer, maxDepth, x, y).first;
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

std::pair<int, int> greedyMove2() {
	// 如果棋盘已满，返回一个无效的位置
	if (isBoardFull()) {
		return std::make_pair(-1, -1);
	}

	if (history.empty()) {
		return std::make_pair(BOARD_SIZE / 2, BOARD_SIZE / 2);
	}
	else {

		int bestScore = std::numeric_limits<int>::lowest();
		std::pair<int, int> move;

		// 如果是游戏开始，电脑先手，选择棋盘中心
		if (history.empty()) {
			move = std::make_pair(BOARD_SIZE / 2, BOARD_SIZE / 2);
		}
		else {
			// 找到所有可能的落子位置
			std::vector<std::pair<int, int>> candidates;
			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					if (board[i][j] == 0 && isNeighbor(i, j)) {
						candidates.push_back(std::make_pair(i, j));
					}
				}
			}

			// 在所有可能的落子位置中，选择一个最优的位置
			for (auto& candidate : candidates) {
				int x = candidate.first;
				int y = candidate.second;

				// 尝试在这里落子
				board[x][y] = -1;
				int score = assess_for_greedy(role, -role, x, y);
				// 撤销尝试
				board[x][y] = 0;

				if (score > bestScore) {
					bestScore = score;
					move = candidate;
				}
			}
		}
		return move;
	}
}

bool isNeighbor(int x, int y) {
	// 检查一个位置的周围是否有棋子
	for (int i = -3; i <= 3; i++) {
		for (int j = -3; j <= 3; j++) {
			// 跳过自身
			if (i == 0 && j == 0) {
				continue;
			}
			// 如果在棋盘范围内，并且有棋子，返回true
			if (x + i >= 0 && x + i < BOARD_SIZE && y + j >= 0 && y + j < BOARD_SIZE && board[x + i][y + j] != 0) {
				return true;
			}
		}
	}
	return false;
}

std::pair<int, int> greedyMove3(int my_color, int opponent_color, int pos_x, int pos_y) {
	// 获取16个最优点
	auto bestMoves = heuristicEvaluation(my_color, opponent_color, pos_x, pos_y);

	// 返回评估值最高的点
	return bestMoves.front().second;
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

		// 使用队列实现广度优先搜索
		std::queue<std::pair<int, int>> q;
		q.push(std::make_pair(centerX, centerY));

		std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));

		while (!q.empty()) {
			std::pair<int, int> cur = q.front();
			q.pop();

			int x = cur.first;
			int y = cur.second;

			bool isContinue = false;
			if (std::abs(x - centerX) > 6 || std::abs(y - centerY) > 6) {
				isContinue = true;
			}

			if (!isContinue && !visited[x][y] && board[x][y] == 0) {
				// 尝试在这里落子
				board[x][y] = -1;
				int score = assess_for_greedy(role, -role, x, y);
				// 撤销尝试
				board[x][y] = 0;
				if (score > bestScore) {
					bestScore = score;
					move = std::make_pair(x, y);
				}
			}

			// 向周围八个方向搜索
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					// 跳过自身
					if (i == 0 && j == 0) {
						continue;
					}
					// 如果没有访问过，且在棋盘范围内，加入队列
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
		int count = 1; //连子数
		int empty = 0; //空位数
		int block = 0; //堵住数
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
			if ((role == -1 && roleReverse == true) || (role == 1 && roleReverse == false)) { // 玩家
				score += FIVE; 
			}
			else {
			score += FIVE * 3;	// 电脑的连五分更高
			}
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
					if ((role == -1 && roleReverse == true) || (role == 1 && roleReverse == false)) { // 玩家
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

int assess_for_greedy(int my_color, int opponent_color, int i, int j)
{
	int score = assess(my_color, opponent_color, i, j) + assess(opponent_color, my_color, i, j);
	return score;
}

std::pair<int, std::pair<int, int>> minimax(int depth, int alpha, int beta, bool isMaximizingPlayer, int maxDepth) {
	if (depth == maxDepth || isBoardFull()) {
		return std::make_pair(assess(role, -role, key_x, key_y) - assess(-role, role, key_x, key_y), std::make_pair(key_x, key_y));
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
			int score = minimax(depth + 1, alpha, beta, !isMaximizingPlayer, maxDepth).first;
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

		// 向周围八个方向搜索
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				// 跳过自身
				if (i == 0 && j == 0) {
					continue;
				}
				// 如果没有访问过，且在棋盘范围内，加入队列
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
		int maxDepth = 6;
		std::pair<int, std::pair<int, int>> result;
		if (history.size() < 8) {
			result.second = greedyMove();
			//result.second = greedyMove2();
			//result.second =  greedyMove3(role, -role, key_x, key_y);
		}
		else {
			result = minimax(1, (std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), true, maxDepth);
			//result = minimax2(computerColor, playerColor, 1, (std::numeric_limits<int>::min)(), (std::numeric_limits<int>::max)(), true, maxDepth, key_x, key_y);
		}
		// 在最优的位置落子
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
	piece_count = 0;  // 重置棋子数量
	while (1) {
		show_board();
		draw_sidebar();
		if (role == 1) {
			get_key();
			piece_count++;  // 增加棋子数量
		}
		else {
			computerMove();
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