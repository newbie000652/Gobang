// game.cpp
#include "game.h"
#include "board.h"
#include "input.h"


#define FIVE 2000000 //����
#define HUO_FOUR 2000000 //����
#define CHONG_FOUR 3000 //����
#define HUO_THREE 3000 //����
#define MIAN_THREE 100 //����
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

void greedyMove() {
	if (isBoardFull()) {
		return;
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

		// ����һ����ά������������¼ÿ��λ���Ƿ��Ѿ������ʹ�
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
				// ��������������
				board[x][y] = -1;
				int score = small_assess(role, -role, x, y) - small_assess(-role, role, x, y);
				// ��������
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

	// �����ŵ�λ������
	board[move.first][move.second] = -1;
	key_x = move.first;
	key_y = move.second;
	history.push(move);
}

int small_assess(int my_color, int opponent_color, int i, int j)
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
	for (int k = 0; k < 8; k++) { //�����˸�����
		int count = 1; //������
		int empty = 0; //��λ��
		int block = 0; //��ס��
		int x = i + direction[k][0]; //���ŷ����ƶ�
		int y = j + direction[k][1];
		while (board[x][y] == my_color) { //������Լ������ӣ���������һ
			count++;
			x += direction[k][0];
			y += direction[k][1];
		}
		if (board[x][y] == 0) { //����ǿ�λ����λ����һ
			empty++;
		}
		else { //����Ƕ��ֵ����ӻ��߳��磬��ס����һ
			block++;
		}
		x = i - direction[k][0]; //�����ƶ�
		y = j - direction[k][1];
		while (board[x][y] == my_color) { //������Լ������ӣ���������һ
			count++;
			x -= direction[k][0];
			y -= direction[k][1];
		}
		if (board[x][y] == 0) { //����ǿ�λ����λ����һ
			empty++;
		}
		else { //����Ƕ��ֵ����ӻ��߳��磬��ס����һ
			block++;
		}
		//��������������λ���Ͷ�ס���ж����Σ�������
		if (count == 5) { //����
			score += FIVE;
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
					score += HUO_THREE;
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

		// �����ŵ�λ������
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
			int score = minimax(depth - 1, alpha, beta, !isMaximizingPlayer).first;
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
			// �������
			get_key();
		}
		else {
			// �������ӣ������㷨��ѡ��һ�ּ���
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
	settextstyle(30, 0, _T("����"));
	outtextxy(260, 160, _T("˫�˶�ս"));
	outtextxy(260, 260, _T("�˻���ս"));
	outtextxy(260, 360, _T("�˳���Ϸ"));
}