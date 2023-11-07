
#include <graphics.h>
#include <conio.h>
#include <stdio.h>

#define SIZE 15
#define BOARD_SIZE (SIZE + 1)  // 新增这行代码
#define GRID_SIZE 40
#define OFFSET 30  
#define LINE_WIDTH 2

int board[BOARD_SIZE][BOARD_SIZE] = { 0 };  // 修改这行代码
int key_x = SIZE / 2, key_y = SIZE / 2;
int role = 1;


void init_board(void);
void show_board(void);
void get_key(void);
bool check_key(void);

int main(void) {
    initgraph(SIZE * GRID_SIZE + OFFSET * 2, SIZE * GRID_SIZE + OFFSET * 2);
    init_board();
    while (1) {
        show_board();
        get_key();
        if (check_key()) {
            show_board();
            printf("%c wins!\n", role == 1 ? 'X' : 'O');
            _getch();
            closegraph();
            return 0;
        }
        role = -role;
    }
}

void init_board(void) {
    for (int i = 0; i < BOARD_SIZE; i++) {  // 修改这行代码
        for (int j = 0; j < BOARD_SIZE; j++) {  // 修改这行代码
            board[i][j] = 0;
        }
    }
}

void show_board(void) {
    setbkcolor(WHITE);
    cleardevice();
    setcolor(BLACK);
    setlinestyle(PS_SOLID, LINE_WIDTH);
    for (int i = 0; i < BOARD_SIZE; i++) {  // 修改这行代码
        line(OFFSET, OFFSET + i * GRID_SIZE, SIZE * GRID_SIZE + OFFSET, OFFSET + i * GRID_SIZE);
        line(OFFSET + i * GRID_SIZE, OFFSET, OFFSET + i * GRID_SIZE, SIZE * GRID_SIZE + OFFSET);
    }
    rectangle(OFFSET, OFFSET, SIZE * GRID_SIZE + OFFSET, SIZE * GRID_SIZE + OFFSET);
    for (int i = 0; i < BOARD_SIZE; i++) {  // 修改这行代码
        for (int j = 0; j < BOARD_SIZE; j++) {  // 修改这行代码
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
    setfillcolor(RED);
    solidcircle(OFFSET + key_x * GRID_SIZE, OFFSET + key_y * GRID_SIZE, GRID_SIZE / 2 - 2);
}

void get_key(void) {
    while (1) {
        switch (_getche()) {
        case 'w': if (key_y > 0) key_y--; break;
        case 's': if (key_y < SIZE) key_y++; break;
        case 'a': if (key_x > 0) key_x--; break;
        case 'd': if (key_x < SIZE) key_x++; break;
        case 'j':
            if (board[key_x][key_y] == 0) {
                board[key_x][key_y] = role;
                return;
            }
        }
        show_board();
    }
}



bool check_key(void) {
    int dx[4] = { -1, 0, 1, 1 };
    int dy[4] = { 1, 1, 1, 0 };
    for (int i = 0; i < 4; i++) {
        int count = 0;
        for (int x = key_x, y = key_y; x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE && board[x][y] == role; x += dx[i], y += dy[i]) {  // 修改这行代码
            count++;
        }
        for (int x = key_x - dx[i], y = key_y - dy[i]; x >= 0 && y >= 0 && x < BOARD_SIZE && y < BOARD_SIZE && board[x][y] == role; x -= dx[i], y -= dy[i]) {  // 修改这行代码
            count++;
        }
        if (count >= 5) {
            return true;
        }
    }
    return false;
}

