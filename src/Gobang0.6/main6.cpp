#if 0

#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stack>

#define SIZE 15
#define BOARD_SIZE (SIZE + 1)
#define GRID_SIZE 40
#define OFFSET 30  
#define LINE_WIDTH 2
#define SIDEBAR_WIDTH 200

int board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int key_x = SIZE / 2, key_y = SIZE / 2;
int role = 1;
std::stack<std::pair<int, int>> history;  // ���ڱ���ÿһ�����ӵ�λ��

void init_board(void);
void show_board(void);
void get_key(void);
bool check_key(void);
void draw_sidebar(void);
void undo(void);  // ���庯��

int main(void) {
	initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
	init_board();
	while (1) {
		show_board();
		draw_sidebar();
		get_key();
		if (check_key()) {
			show_board();
			settextstyle(32, 0, _T("����"));
			outtextxy(SIZE * GRID_SIZE / 2, SIZE * GRID_SIZE / 2, role == 1 ? _T("���ӷ�ʤ����") : _T("���ӷ�ʤ����"));
			_getch();
			closegraph();
			return 0;
		}
		role = -role;
	}
}

void init_board(void) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = 0;
		}
	}
}

void get_key(void) {
	while (1) {
		MOUSEMSG msg = GetMouseMsg();
		if (msg.uMsg == WM_LBUTTONDOWN) {
			int x = (msg.x - OFFSET + GRID_SIZE / 2) / GRID_SIZE;
			int y = (msg.y - OFFSET + GRID_SIZE / 2) / GRID_SIZE;
			if (x >= 0 && x <= SIZE && y >= 0 && y <= SIZE && board[x][y] == 0) {
				board[x][y] = role;
				key_x = x;  // ���� key_x ��ֵ
				key_y = y;  // ���� key_y ��ֵ
				history.push(std::make_pair(x, y));  // ������λ�ñ��浽ջ��
				show_board();
				return;
			}
			else if (x >= SIZE + 2 && x <= SIZE + 2 + SIDEBAR_WIDTH / GRID_SIZE && y >= SIZE / 2 && y <= SIZE / 2 + 50 / GRID_SIZE) {
				undo();  // ������尴ť
				show_board();
				return;
			}
		}
	}
}

void undo(void) {
	if (!history.empty()) {
		std::pair<int, int> last_move = history.top();
		history.pop();
		board[last_move.first][last_move.second] = 0;
		//role = -role;  // �л���ɫ
	}
}


void show_board(void) {
	setbkcolor(YELLOW);
	cleardevice();
	setcolor(BLACK);
	setlinestyle(PS_SOLID, LINE_WIDTH);
	for (int i = 0; i < BOARD_SIZE; i++) {
		line(OFFSET, OFFSET + i * GRID_SIZE, SIZE * GRID_SIZE + OFFSET, OFFSET + i * GRID_SIZE);
		line(OFFSET + i * GRID_SIZE, OFFSET, OFFSET + i * GRID_SIZE, SIZE * GRID_SIZE + OFFSET);
	}
	rectangle(OFFSET, OFFSET, SIZE * GRID_SIZE + OFFSET, SIZE * GRID_SIZE + OFFSET);
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == 1) {
				setfillcolor(BLACK);
				solidcircle(OFFSET + i * GRID_SIZE, OFFSET + j * GRID_SIZE, GRID_SIZE / 2 - 2);
			}
			else if (board[i][j] == -1) {
				setfillcolor(WHITE);
				solidcircle(OFFSET + i * GRID_SIZE, OFFSET + j * GRID_SIZE, GRID_SIZE / 2 - 2);
			}
		}
	}
}


bool check_key(void) {
	int dx[4] = { -1, 0, 1, 1 };
	int dy[4] = { 1, 1, 1, 0 };
	for (int i = 0; i < 4; i++) {
		int count = 0;
		for (int x = key_x, y = key_y; x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE && board[x][y] == role; x += dx[i], y += dy[i]) {
			count++;
		}
		for (int x = key_x - dx[i], y = key_y - dy[i]; x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE && board[x][y] == role; x -= dx[i], y -= dy[i]) {
			count++;
		}
		if (count >= 5) {
			return true;
		}
	}
	return false;
}

void draw_sidebar(void) {

	setfillcolor(LIGHTRED);
	solidrectangle(SIZE * GRID_SIZE + OFFSET * 2, 0, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);


	setfillcolor(RED);
	solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, SIZE * GRID_SIZE / 2, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, SIZE * GRID_SIZE / 2 + 50);


	settextstyle(32, 0, _T("����"));
	settextcolor(GREEN);
	outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 60, SIZE * GRID_SIZE / 2 + 10, _T("����"));


	setlinestyle(PS_SOLID, 5);
	setcolor(RED);
	line(SIZE * GRID_SIZE + OFFSET * 2, 0, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, 0);
}





#endif
