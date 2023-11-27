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
		showResult(role == 1 ? _T("����ʤ��!") : _T("����ʤ��!"));
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

int calculate(int* position) {
	int sum = 0;
	for (int i = 0; i < 36; i++) {
		if (std::equal(position, position + 4, situation[i])) {
			sum += baseScoreTable[i];
			if (role == 1 && i % 2 == 0) {
				sum += bonusScoreTable[i];
			}
			else if (role != 1 && i % 2 == 1) {
				sum += bonusScoreTable[i];
			}
			break;
		}
	}
	return sum;
}

int calculate2(int* position) {
	int sum = 0;
	for (int i = 0; i < 18; i++) {
		if (std::equal(position, position + 4, situation2[i])) {
			sum += baseScoreTable2[i];
			if (role == 1 && i % 2 == 0) {
				sum += bonusScoreTable2[i];
			}
			else if (role != 1 && i % 2 == 1) {
				sum += bonusScoreTable2[i];
			}
			break;
		}
	}
	return sum;
}


int mark(int x, int y, int dx, int dy, int steps) {
    int sum = 0;
    int position[4];
    for (int step = 1; step <= steps; step++) {
        for (int i = -2; i <= 1; i++) {
            int nx = x + dx * (i + step);
            int ny = y + dy * (i + step);
            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                position[i + 2] = board[nx][ny];
            }
            else {
                position[i + 2] = 0;
            }
        }
        sum += calculate2(position);
    }
    return sum;
}

int mark1(int x, int y) {
    return mark(x, y, 1, 0, 1);
}

int mark2(int x, int y) {
    return mark(x, y, 0, 1, 1);
}

int mark3(int x, int y) {
    return mark(x, y, 1, 1, 1);
}

int mark4(int x, int y) {
    return mark(x, y, -1, 1, 1);
}

int mark5(int x, int y) {
    return mark(x, y, 0, 0, 5);
}


void computerMove() {
	if (isBoardFull()) {
		return;
	}

	int bestScore = -INFINITY;
	std::pair<int, int> move;
	move.first = -1;
	move.second = -1;

	// �ҵ���һ���յ�λ����Ϊ��ʼ�����λ��
	int center = BOARD_SIZE / 2;
	for (int layer = 0; layer <= center; layer++) {
		for (int i = center - layer; i <= center + layer; i++) {
			for (int j = center - layer; j <= center + layer; j++) {
				if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE && board[i][j] == 0) {
					move = std::make_pair(i, j);
					bestScore = minimax(2, -999999, 999999, true); // �����ʼλ�õķ���
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
					// ��������������
					board[i][j] = -1;
					int score = evaluate_position(i, j);
					// ��������
					board[i][j] = 0;
					if (score > bestScore) {
						bestScore = score;
						move = std::make_pair(i, j);
					}
				}
			}
		}
	}

	// �����ŵ�λ������
	board[move.first][move.second] = -1;
	key_x = move.first;
	key_y = move.second;
	history.push(move);
}

int getChessType(int x, int y, int dx, int dy) {
	int position[4];
	for (int i = 0; i < 4; i++) {
		if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
			position[i] = board[x][y];
		}
		else {
			position[i] = 0;
		}
		x += dx;
		y += dy;
	}
	for (int i = 0; i < 18; i++) {
		if (std::equal(position, position + 4, situation2[i])) {
			return i;
		}
	}
	return -1;
}


double position_weight(int x, int y) {
	return weights[x][y];
}

int evaluate_position(int x, int y) {
	int score = 0;
	// �������λ�õİ˸�����
	score += mark1(x, y);
	score += mark2(x, y);
	score += mark3(x, y);
	score += mark4(x, y);
	score += mark5(x, y);
	return score;
}

int evaluate() {
	int score = 0;
	// ���������ϵ�ÿ��λ��
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == role) {
				// ������λ���ǵ��Ե����ӣ����ӷ���
				score += evaluate_position(i, j);
			}
			else if (board[i][j] == -role) {
				// ������λ������ҵ����ӣ����ٷ���
				score -= evaluate_position(i, j);
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

	// �ҵ���һ���յ�λ����Ϊ��ʼ�����λ��
	int center = BOARD_SIZE / 2;
	for (int layer = 0; layer <= center; layer++) {
		for (int i = center - layer; i <= center + layer; i++) {
			for (int j = center - layer; j <= center + layer; j++) {
				if (i >= 0 && i < BOARD_SIZE && j >= 0 && j < BOARD_SIZE && board[i][j] == 0) {
					move = std::make_pair(i, j);
					bestScore = evaluate_position(i, j); // �����ʼλ�õķ���
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
					// ��������������
					board[i][j] = -1;
					int score = evaluate_position(i, j);
					// ��������
					board[i][j] = 0;
					if (score > bestScore) {
						bestScore = score;
						move = std::make_pair(i, j);
					}
				}
			}
		}
	}

	// �����ŵ�λ������
	board[move.first][move.second] = -1;
	key_x = move.first;
	key_y = move.second;
	history.push(move);
}


int minimax(int depth, int alpha, int beta, bool isMaximizingPlayer) {
	if (depth == 0) {
		return evaluate();
	}

	if (isMaximizingPlayer) {
		int maxEval = -999999;
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (board[i][j] == 0) {
					// ��������������
					board[i][j] = -1;
					int eval = minimax(depth - 1, alpha, beta, false);
					// ��������
					board[i][j] = 0;
					maxEval = maxEval > eval ? maxEval : eval;
					alpha = alpha > eval ? alpha : eval;
					if (beta <= alpha) {
						return maxEval;
					}
				}
			}
		}
		return maxEval;
	}
	else {
		int minEval = 999999;
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				if (board[i][j] == 0) {
					// ��������������
					board[i][j] = 1;
					int eval = minimax(depth - 1, alpha, beta, true);
					// ��������
					board[i][j] = 0;
					minEval = minEval > eval ? eval : minEval;
					beta = beta > eval ? eval : beta;
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
	setbkcolor(WHITE);
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