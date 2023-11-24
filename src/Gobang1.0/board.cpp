// board.cpp
#include "board.h"

int board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int key_x = SIZE / 2, key_y = SIZE / 2;
int role = 1;
std::stack<std::pair<int, int>> history;

void init_board(void) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = 0;
        }
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

std::pair<bool, std::pair<int, int>> check_key(int pawn_num) {
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
        if (count >= pawn_num) {
            return std::make_pair(true, std::make_pair(key_x + dx[i] * (pawn_num - 1), key_y + dy[i] * (pawn_num - 1)));
        }
    }
    return std::make_pair(false, std::make_pair(-1, -1));
}

void draw_sidebar(void) {

    setfillcolor(LIGHTRED);
    solidrectangle(SIZE * GRID_SIZE + OFFSET * 2, 0, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);


    setfillcolor(RED);
    solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, SIZE * GRID_SIZE / 2, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, SIZE * GRID_SIZE / 2 + 50);


    settextstyle(32, 0, _T("ËÎÌå"));
    settextcolor(GREEN);
    outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 60, SIZE * GRID_SIZE / 2 + 10, _T("»ÚÆå"));


    setlinestyle(PS_SOLID, 5);
    setcolor(RED);
    line(SIZE * GRID_SIZE + OFFSET * 2, 0, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, 0);
}

void undo(void) {
    if (!history.empty()) {
        std::pair<int, int> last_move = history.top();
        history.pop();
        board[last_move.first][last_move.second] = 0;
        //role = -role;  // Switch roles
    }
}
