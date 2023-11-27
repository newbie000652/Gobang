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
                key_x = x;  
                key_y = y;  
                history.push(std::make_pair(x, y));  
                show_board();
                return;
            }
            else if (msg.x >= SIZE * GRID_SIZE + OFFSET * 2 + 20 && msg.x <= SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20 && msg.y >= SIZE * GRID_SIZE / 2 && msg.y <= SIZE * GRID_SIZE / 2 + 50) {
                if (getGameMode() == 0) {
                    undo(1); 
                }
                else {
                    undo(2);
                    role = -role;       // 又是这个奇奇怪怪的 bug
                }
                show_board();
                return;
            }
            else if (msg.x >= SIZE * GRID_SIZE + OFFSET * 2 + 20 && msg.x <= SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20 && msg.y >= SIZE * GRID_SIZE / 2 + 60 && msg.y <= SIZE * GRID_SIZE / 2 + 110) {
                handleMenuSelection();
                return;
            }
        }
    }
}


