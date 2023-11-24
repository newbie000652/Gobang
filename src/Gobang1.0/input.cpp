// input.cpp
#include "input.h"
#include "game.h"
#include "board.h"

void processMouseClick(ExMessage msg) {
    if (msg.x >= 220 && msg.x <= 420 && msg.y >= 150 && msg.y <= 200)
        startDoublePlayerBattle();
    else if (msg.x >= 220 && msg.x <= 420 && msg.y >= 250 && msg.y <= 300)
        startSinglePlayerBattle();
    else if (msg.x >= 220 && msg.x <= 420 && msg.y >= 350 && msg.y <= 400)
        terminateGame();
}

void processKeyboardInput() {
    _getch();
}

void get_key(void) {
    while (1) {
        MOUSEMSG msg = GetMouseMsg();
        if (msg.uMsg == WM_LBUTTONDOWN) {
            int x = (msg.x - OFFSET + GRID_SIZE / 2) / GRID_SIZE;
            int y = (msg.y - OFFSET + GRID_SIZE / 2) / GRID_SIZE;
            if (x >= 0 && x <= SIZE && y >= 0 && y <= SIZE && board[x][y] == 0) {
                board[x][y] = role;
                key_x = x;  // Update the value of key_x
                key_y = y;  // Update the value of key_y
                history.push(std::make_pair(x, y));  // Save the position of the chess piece in the stack
                show_board();
                return;
            }
            else if (x >= SIZE + 2 && x <= SIZE + 2 + SIDEBAR_WIDTH / GRID_SIZE && y >= SIZE / 2 && y <= SIZE / 2 + 50 / GRID_SIZE) {
                undo();  // Click the undo button
                show_board();
                return;
            }
        }
    }
}
