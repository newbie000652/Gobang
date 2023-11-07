
// 低配版本五子棋0.1
#pragma warning(disable : 4996)		// 关闭微软安全函数警告。
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

char board[15][15];
char key_x = 7, key_y = 7;
char role = '@';

void init_board(void);		// 初始化棋盘
void show_board(void);		// 显示棋盘
void get_key(void);			// 落子
int count_key(int go_x, int go_y);	// 检查一个方向上有多少个相同的棋子
bool check_key(void);		// 检查五子连珠

int main(void)
{
	init_board();
	while (1) {
		show_board();
		get_key();
		if (check_key()) {
			show_board();
			printf("%c胜利！\n", role);
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
	printf("请 %c 下棋：\n", role);
	while (1) {
		switch (getch()) {
		case 'w':
			if (key_x - 1 > 0) {
				if (board[key_x - 1][key_y] == '*') {
					key_x--;
					board[key_x][key_y] = '#';
					show_board();
					printf("请 %c 下棋：\n", role);
					board[key_x][key_y] = '*';
					break;
				}
				else if (board[key_x - 1][key_y] == '@') {
					key_x--;
					board[key_x][key_y] = '#';
					show_board();
					printf("请 %c 下棋：\n", role);
					board[key_x][key_y] = '@';
					break;
				}
				else if (board[key_x - 1][key_y] == '$') {
					key_x--;
					board[key_x][key_y] = '#';
					show_board();
					printf("请 %c 下棋：\n", role);
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
					printf("请 %c 下棋：\n", role);
					board[key_x][key_y] = '*';
					break;
				}
				else if (board[key_x + 1][key_y] == '@') {
					key_x++;
					board[key_x][key_y] = '#';
					show_board();
					printf("请 %c 下棋：\n", role);
					board[key_x][key_y] = '@';
					break;
				}
				else if (board[key_x + 1][key_y] == '$') {
					key_x++;
					board[key_x][key_y] = '#';
					show_board();
					printf("请 %c 下棋：\n", role);
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
					printf("请 %c 下棋：\n", role);
					board[key_x][key_y] = '*';
					break;
				}
				else if (board[key_x][key_y + 1] == '@') {
					key_y++;
					board[key_x][key_y] = '#';
					show_board();
					printf("请 %c 下棋：\n", role);
					board[key_x][key_y] = '@';
					break;
				}
				else if (board[key_x][key_y + 1] == '$') {
					key_y++;
					board[key_x][key_y] = '#';
					show_board();
					printf("请 %c 下棋：\n", role);
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
					printf("请 %c 下棋：\n", role);
					board[key_x][key_y] = '*';
					break;
				}
				else if (board[key_x][key_y - 1] == '@') {
					key_y--;
					board[key_x][key_y] = '#';
					show_board();
					printf("请 %c 下棋：\n", role);
					board[key_x][key_y] = '@';
					break;
				}
				else if (board[key_x][key_y - 1] == '$') {
					key_y--;
					board[key_x][key_y] = '#';
					show_board();
					printf("请 %c 下棋：\n", role);
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
