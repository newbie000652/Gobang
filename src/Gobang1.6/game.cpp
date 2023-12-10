// game.cpp
#include "game.h"
#include "board.h"
#include "input.h"

#define FIVE 90000 //����
#define HUO_FOUR 40000 //����
#define CHONG_FOUR 2000 //����
#define HUO_THREE 3000 //����
#define MIAN_THREE 80 //����
#define HUO_TWO 100 //���
#define MIAN_TWO 10 //�߶�
#define ONE 1 //��һ


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
	settextstyle(32, 0, _T("����"));
	settextcolor(GREEN);
	outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 30, 30, resultText);
	//_getch();     // ����ע�͵���Ϊ����Ϸ��������ʾ�˵�
	//closegraph();
}

void startDoublePlayerBattle() {
	setGameMode(HUMAN_VS_HUMAN);
	initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
	init_board();
	piece_count = 0;  // ������������
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
			showResult(role == 1 ? _T("����ʤ��!") : _T("����ʤ��!"));
		}
		else {
			showResult(role == -1 ? _T("����ʤ��!") : _T("����ʤ��!"));
		}
		handleMenuSelection();
		return true;
	}
	if (isBoardFull()) {
		showResult(_T("ƽ�֣�"));
		handleMenuSelection();
		return true;
	}
	return false;
}

std::vector<std::pair<int, std::pair<int, int>>> heuristicEvaluation(int my_color, int opponent_color, int pos_x, int pos_y) {
	// ����һ�� multimap ���洢����ֵ�Ͷ�Ӧ��λ��
	std::multimap<int, std::pair<int, int>, std::greater<int>> mm;

	// ����һ���������ڹ����������
	std::queue<std::pair<int, int>> q;
	q.push(std::make_pair(pos_x, pos_y));

	// ����һ����ά������������¼ÿ��λ���Ƿ��Ѿ������ʹ�
	std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));

	while (!q.empty()) {
		std::pair<int, int> cur = q.front();
		q.pop();

		int x = cur.first;
		int y = cur.second;

		// ֻ�Կ�λ�ý�������
		if (!visited[x][y] && board[x][y] == 0) {
			// ��������������
			board[x][y] = my_color;
			int score = assess(my_color, opponent_color, x, y) + assess(opponent_color, my_color, x, y);
			// ��������
			board[x][y] = 0;

			// ������ֵ�Ͷ�Ӧ��λ�ü��� multimap
			mm.insert(std::make_pair(score, std::make_pair(x, y)));
		}

		// ����Χ�˸���������
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				// ��������
				if (dx == 0 && dy == 0) {
					continue;
				}
				// ���û�з��ʹ����������̷�Χ�ڣ��������
				if (x + dx >= 0 && x + dx < BOARD_SIZE && y + dy >= 0 && y + dy < BOARD_SIZE && !visited[x + dx][y + dy]) {
					q.push(std::make_pair(x + dx, y + dy));
				}
			}
		}

		visited[x][y] = true;
	}

	// �� multimap �е�Ԫ��ת�Ƶ�һ��������
	std::vector<std::pair<int, std::pair<int, int>>> bestMoves;
	for (const auto& pair : mm) {
		bestMoves.push_back(pair);
		// ��������Ĵ�С����16����ֹͣ���
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

	// ��ȡ16�����ŵ�
	auto bestMoves = heuristicEvaluation(isMaximizingPlayer ? -1 : 1, isMaximizingPlayer ? 1 : -1, pos_x, pos_y);

	// Ԥ��������ÿ����ѡ�����һ��ǳ��ε���������ȡ����ֵ
	std::map<int, std::pair<int, int>, std::greater<int>> preScores;
	for (const auto& bestMove : bestMoves) {
		int x = bestMove.second.first;
		int y = bestMove.second.second;

		board[x][y] = isMaximizingPlayer ? my_color : opponent_color;
		int score = assess(my_color, opponent_color, x, y) - assess(opponent_color, my_color, x, y);
		board[x][y] = 0;

		preScores.insert(std::make_pair(score, std::make_pair(x, y)));
	}

	// �������ĺ�ѡ������������
	for (const auto& preScore : preScores) {
		int x = preScore.second.first;
		int y = preScore.second.second;

		board[x][y] = isMaximizingPlayer ? my_color : opponent_color;

		// �����������
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
	// �����������������һ����Ч��λ��
	if (isBoardFull()) {
		return std::make_pair(-1, -1);
	}

	if (history.empty()) {
		return std::make_pair(BOARD_SIZE / 2, BOARD_SIZE / 2);
	}
	else {

		int bestScore = std::numeric_limits<int>::lowest();
		std::pair<int, int> move;

		// �������Ϸ��ʼ���������֣�ѡ����������
		if (history.empty()) {
			move = std::make_pair(BOARD_SIZE / 2, BOARD_SIZE / 2);
		}
		else {
			// �ҵ����п��ܵ�����λ��
			std::vector<std::pair<int, int>> candidates;
			for (int i = 0; i < BOARD_SIZE; i++) {
				for (int j = 0; j < BOARD_SIZE; j++) {
					if (board[i][j] == 0 && isNeighbor(i, j)) {
						candidates.push_back(std::make_pair(i, j));
					}
				}
			}

			// �����п��ܵ�����λ���У�ѡ��һ�����ŵ�λ��
			for (auto& candidate : candidates) {
				int x = candidate.first;
				int y = candidate.second;

				// ��������������
				board[x][y] = -1;
				int score = assess_for_greedy(role, -role, x, y);
				// ��������
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
	// ���һ��λ�õ���Χ�Ƿ�������
	for (int i = -3; i <= 3; i++) {
		for (int j = -3; j <= 3; j++) {
			// ��������
			if (i == 0 && j == 0) {
				continue;
			}
			// ��������̷�Χ�ڣ����������ӣ�����true
			if (x + i >= 0 && x + i < BOARD_SIZE && y + j >= 0 && y + j < BOARD_SIZE && board[x + i][y + j] != 0) {
				return true;
			}
		}
	}
	return false;
}

std::pair<int, int> greedyMove3(int my_color, int opponent_color, int pos_x, int pos_y) {
	// ��ȡ16�����ŵ�
	auto bestMoves = heuristicEvaluation(my_color, opponent_color, pos_x, pos_y);

	// ��������ֵ��ߵĵ�
	return bestMoves.front().second;
}

std::pair<int, int> greedyMove() {
	if (isBoardFull()) {
		return std::make_pair(-1, -1);
	}

	int bestScore = std::numeric_limits<int>::lowest();
	std::pair<int, int> move;

	// �������Ϸ��ʼ���������֣�ѡ����������
	if (history.empty()) {
		move = std::make_pair(BOARD_SIZE / 2, BOARD_SIZE / 2);
	}
	else {
		// �ҵ��������ӵ�λ��
		int centerX = key_x;
		int centerY = key_y;

		// ʹ�ö���ʵ�ֹ����������
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
				// ��������������
				board[x][y] = -1;
				int score = assess_for_greedy(role, -role, x, y);
				// ��������
				board[x][y] = 0;
				if (score > bestScore) {
					bestScore = score;
					move = std::make_pair(x, y);
				}
			}

			// ����Χ�˸���������
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					// ��������
					if (i == 0 && j == 0) {
						continue;
					}
					// ���û�з��ʹ����������̷�Χ�ڣ��������
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
	{-1, -1}, //����
	{-1, 0}, //��
	{-1, 1}, //����
	{0, -1}, //��
	{0, 1}, //��
	{1, -1}, //����
	{1, 0}, //��
	{1, 1} //����
	};

	//����1/4���̵�λ������
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
		int count = 1; //������
		int empty = 0; //��λ��
		int block = 0; //��ס��
		int x = i + direction[k][0]; //���ŷ����ƶ�
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
		x = i - direction[k][0]; //�����ƶ�
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
		//��������������λ���Ͷ�ס���ж����Σ�������
		if (count >= 5) { //����
			if ((role == -1 && roleReverse == true) || (role == 1 && roleReverse == false)) { // ���
				score += FIVE; 
			}
			else {
			score += FIVE * 3;	// ���Ե�����ָ���
			}
		}
		else if (count == 4) { //����
			if (empty == 2) { //����
				score += HUO_FOUR;
			}
			else if (empty == 1) { //����
				score += CHONG_FOUR;
			}
		}
		else if (count == 3) { //����
			if (empty == 2) { //����
				if (block == 0) { //˫����
					score += HUO_THREE * 2;
				}
				else { //������
					if ((role == -1 && roleReverse == true) || (role == 1 && roleReverse == false)) { // ���
						score += HUO_THREE * 1.2;
					}
					else {
						score += HUO_THREE;
					}
				}
			}
			else if (empty == 1) { //����
				score += MIAN_THREE;
			}
		}
		else if (count == 2) { //����
			if (empty == 2) { //���
				score += HUO_TWO;
			}
			else if (empty == 1) { //�߶�
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

	int noImprovementCounter = 0; // ���һ������������������δ�ҵ����ŵ�Ĵ���

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
				noImprovementCounter = 0; // ����ҵ��˸��ŵĵ㣬���ü�����
			}
			else if (!isMaximizingPlayer && score < bestScore) {
				bestScore = score;
				move = cur;
				beta = std::fmin(beta, score);
				noImprovementCounter = 0; // ����ҵ��˸��ŵĵ㣬���ü�����
			}
			else {
				noImprovementCounter++; // ���û���ҵ����ŵĵ㣬���Ӽ�����
			}

			if (beta <= alpha || noImprovementCounter >= 36) { // �������36��û���ҵ����ŵĵ㣬��ǰ����ѭ��
				break;
			}
		}

		// ����Χ�˸���������
		for (int i = -1; i <= 1; i++) {
			for (int j = -1; j <= 1; j++) {
				// ��������
				if (i == 0 && j == 0) {
					continue;
				}
				// ���û�з��ʹ����������̷�Χ�ڣ��������
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
		// �����ŵ�λ������
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
	piece_count = 0;  // ������������
	while (1) {
		show_board();
		draw_sidebar();
		if (role == 1) {
			get_key();
			piece_count++;  // ������������
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
	settextstyle(30, 0, _T("����"));
	outtextxy(260, 160, _T("˫�˶�ս"));
	outtextxy(260, 260, _T("�˻���ս"));
	outtextxy(260, 360, _T("�˳���Ϸ"));
}