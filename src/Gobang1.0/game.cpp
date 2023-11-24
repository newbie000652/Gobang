// game.cpp
#include "game.h"
#include "board.h"
#include "input.h"

void startDoublePlayerBattle() {
    initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
    init_board();
    while (1) {
        show_board();
        draw_sidebar();
        get_key();
        std::pair<bool, std::pair<int, int>> result = check_key(5);
        if (result.first) {
            show_board();
            settextstyle(32, 0, _T("宋体"));
            outtextxy(SIZE * GRID_SIZE / 2, SIZE * GRID_SIZE / 2, role == 1 ? _T("黑子方胜利！") : _T("白子方胜利！"));
            _getch();
            closegraph();
            return;
        }
        role = -role;
    }
}

void computerMove() {
    int dx[8] = { -1, 0, 1, 1, 1, 0, -1, -1 };
    int dy[8] = { 1, 1, 1, 0, -1, -1, -1, 0 };

    // 获取玩家最新的落子位置
    int last_x = history.top().first;
    int last_y = history.top().second;

    // 检查玩家最新落子的周围八个方向
    for (int k = 0; k < 8; k++) {
        int x = last_x + dx[k];
        int y = last_y + dy[k];

        // 检查是否在棋盘内，且未落子
        if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y] == 0) {
            // 尝试在这里落子
            board[x][y] = -1;

            // 检查玩家是否有四子连线
            std::pair<bool, std::pair<int, int>> result = check_key(4);
            if (result.first) {
                // 如果有，则在这里落子阻止
                key_x = result.second.first;
                key_y = result.second.second;
                history.push(std::make_pair(key_x, key_y));
                return;
            }

            // 撤销尝试
            board[x][y] = 0;
        }
    }

    // 如果没有需要阻止的四子连线，就在玩家的棋子周围随机落子
    while (true) {
        int i = rand() % BOARD_SIZE;
        int j = rand() % BOARD_SIZE;

        // 检查是否在玩家的棋子周围
        for (int k = 0; k < 8; k++) {
            int x = i + dx[k];
            int y = j + dy[k];
            if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y] == 1) {
                // 检查所选位置是否为空
                if (board[i][j] == 0) {
                    // 尝试在这里落子
                    board[i][j] = -1;

                    // 检查玩家是否有三子连线
                    std::pair<bool, std::pair<int, int>> result = check_key(3);
                    if (result.first) {
                        // 如果有，则在这里落子阻止
                        key_x = result.second.first;
                        key_y = result.second.second;
                        history.push(std::make_pair(key_x, key_y));
                        return;
                    }
                    else {
                        // 如果没有，则保留这个落子
                        key_x = i;
                        key_y = j;
                        history.push(std::make_pair(i, j));
                        return;
                    }
                }
            }
        }
    }
}


void startSinglePlayerBattle() {
    initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
    init_board();
    while (1) {
        show_board();
        draw_sidebar();
        if (role == 1) {
            // 玩家落子
            get_key();
        }
        else {
            // 电脑落子
            computerMove();
        }
        std::pair<bool, std::pair<int, int>> result = check_key(5);
        if (result.first) {
            show_board();
            settextstyle(32, 0, _T("宋体"));
            outtextxy(SIZE * GRID_SIZE / 2, SIZE * GRID_SIZE / 2, role == 1 ? _T("黑子方胜利！") : _T("白子方胜利！"));
            _getch();
            closegraph();
            return;
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
    settextstyle(30, 0, _T("黑体"));
    outtextxy(260, 160, _T("双人对战"));
    outtextxy(260, 260, _T("人机对战"));
    outtextxy(260, 360, _T("退出游戏"));
}