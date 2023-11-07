#if 0

// ����汾������0.1
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
void get_key(void) {	
	printf("�� %c ���壺\n", role);
	while (1) {
		switch (getch()) {
		case 'w':
			if (key_x - 1 > 0) {
				if (board[key_x - 1][key_y] == '*') {
					key_x--;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '*';
					break;
				}
				else if (board[key_x - 1][key_y] == '@') {
					key_x--;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '@';
					break;
				}
				else if (board[key_x - 1][key_y] == '$') {
					key_x--;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '$';
					break;
				}
			}
			else {
				break;
			}
		case 's':
			if (key_x + 1 > 0) {
				if (board[key_x + 1][key_y] == '*') {
					key_x++;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '*';
					break;
				}
				else if (board[key_x + 1][key_y] == '@') {
					key_x++;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '@';
					break;
				}
				else if (board[key_x + 1][key_y] == '$') {
					key_x++;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '$';
					break;
				}
			}
			else {
				break;
			}
		case 'd':
			if (key_y + 1 > 0) {
				if (board[key_x][key_y + 1] == '*') {
					key_y++;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '*';
					break;
				}
				else if (board[key_x][key_y + 1] == '@') {
					key_y++;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '@';
					break;
				}
				else if (board[key_x][key_y + 1] == '$') {
					key_y++;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '$';
					break;
				}
			}
			else {
				break;
			}
		case 'a':
			if (key_y - 1 > 0) {
				if (board[key_x][key_y - 1] == '*') {
					key_y--;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '*';
					break;
				}
				else if (board[key_x][key_y - 1] == '@') {
					key_y--;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '@';
					break;
				}
				else if (board[key_x][key_y - 1] == '$') {
					key_y--;
					board[key_x][key_y] = '#';
					show_board();
					printf("�� %c ���壺\n", role);
					board[key_x][key_y] = '$';
					break;
				}
			}
			else {
				break;
			}
		case 'j':
			if (board[key_x][key_y] == '*') {
				board[key_x][key_y] = role;
				return;
			}

		}
	}
}
int count_key(int go_x, int go_y) {
	int count = 0;
	int x = key_x + go_x;
	int y = key_y + go_y;
	for (; x >= 0 && y >= 0 && x < 15 && y < 15; x += go_x, y += go_y) {
		if (role == board[x][y]) {
			count++;
		}
		else {
			break;
		}
	}
	return count;
}
bool check_key(void) {
	if (count_key(-1, -1) + count_key(1, 1) >= 4) {
		return true;
	}
	if (count_key(-1, 1) + count_key(1, -1) >= 4) {
		return true;
	}
	if (count_key(1, 0) + count_key(-1, 0) >= 4) {
		return true;
	}
	if (count_key(0, 1) + count_key(0, -1) >= 4) {
		return true;
	}
	return false;
}


#endif