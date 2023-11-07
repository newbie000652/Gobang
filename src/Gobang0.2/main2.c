#if 0

// ����汾������
#pragma warning(disable : 4996)		// �ر�΢��ȫ�������档
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

char board[15][15];
char key_x = 7, key_y = 7;
char role = '@';

void init_board(void);		// ��ʼ������
void show_board(void);		// ��ʾ����
void get_key(void);			// ����
int count_key(int go_x, int go_y);	// ���һ���������ж��ٸ���ͬ������
bool check_key(void);		// �����������

int main(void)
{
	init_board();
	while (1) {
		show_board();
		get_key();
		if (check_key()) {
			show_board();
			printf("%cʤ����\n", role);
			return 0;
		}
		role = (role == '@' ? '$' : '@');
	}
}

void init_board(void) {
	int i, j;
	for (i = 0; i < 15; i++) {
		for (j = 0; j < 15; j++) {
			board[i][j] = '*';
		}
	}
}
void show_board(void) {
	system("cls");
	int i, j;
	for (i = 0; i < 15; i++) {
		for (j = 0; j < 15; j++) {
			printf(" %c", board[i][j]);
		}
		printf("\n");
	}
}

void update_board_and_show(char new_char) {
	char old_char = board[key_x][key_y];
	board[key_x][key_y] = '#';
	show_board();
	printf("�� %c ���壺\n", role);
	board[key_x][key_y] = old_char;
}

void move_key(int delta_x, int delta_y) {
	if (key_x + delta_x >= 0 && key_x + delta_x < 15 && key_y + delta_y >= 0 && key_y + delta_y < 15) {
		char next_pos = board[key_x + delta_x][key_y + delta_y];
		if (next_pos != '#') {
			key_x += delta_x;
			key_y += delta_y;
			update_board_and_show(next_pos);
		}
	}
}

void get_key(void) {
	printf("�� %c ���壺\n", role);
	while (1) {
		switch (getch()) {
		case 'w': move_key(-1, 0); break;
		case 's': move_key(1, 0); break;
		case 'd': move_key(0, 1); break;
		case 'a': move_key(0, -1); break;
		case 'j':
			if (board[key_x][key_y] == '*') {
				board[key_x][key_y] = role;
				return;
			}
		}
	}
}


bool check_line(int dx, int dy) {
	int count = 1; 
	for (int x = key_x + dx, y = key_y + dy; x >= 0 && y >= 0 && x < 15 && y < 15 && board[x][y] == role; x += dx, y += dy) {
		count++;
	}
	for (int x = key_x - dx, y = key_y - dy; x >= 0 && y >= 0 && x < 15 && y < 15 && board[x][y] == role; x -= dx, y -= dy) {
		count++;
	}
	return count >= 5;
}

bool check_key(void) {
	return check_line(-1, -1) || check_line(-1, 1) || check_line(1, 0) || check_line(0, 1);
}



#endif