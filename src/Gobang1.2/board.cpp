// board.cpp
#include "board.h"
#include "game.h"

int board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int key_x = SIZE / 2, key_y = SIZE / 2;
double weights[BOARD_SIZE][BOARD_SIZE] = { 0 };
int role = 1;
bool menuOpen = 0;
std::stack<std::pair<int, int>> history;

void init_board(void) { 
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = 0;
        }
    }
}

void precompute_weights() {
    double center = BOARD_SIZE / 2.0;
    double max_distance = sqrt(2 * center * center); // 对角线距离最大
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            double distance = sqrt((x - center) * (x - center) + (y - center) * (y - center));
            weights[x][y] = 1.0 - distance / max_distance; // 使用最大距离进行归一化
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

    // 状态栏
    setfillcolor(RED);
    solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, 20, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, 70);
    settextstyle(32, 0, _T("宋体"));
    settextcolor(GREEN);
    outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 40, 30, role == 1 ? _T("该黑子啦") : _T("该白子啦"));

    // 悔棋按钮
    setfillcolor(RED);
    solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, SIZE * GRID_SIZE / 2, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, SIZE * GRID_SIZE / 2 + 50);
    settextstyle(32, 0, _T("宋体"));
    settextcolor(GREEN);
    outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 60, SIZE * GRID_SIZE / 2 + 10, _T("悔棋"));

    // 菜单按钮
    setfillcolor(RED);
    solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, SIZE * GRID_SIZE / 2 + 60, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, SIZE * GRID_SIZE / 2 + 110);
    settextstyle(32, 0, _T("宋体"));
    settextcolor(GREEN);
    outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 60, SIZE * GRID_SIZE / 2 + 70, _T("菜单"));

    setlinestyle(PS_SOLID, 5);
    setcolor(RED);
    line(SIZE * GRID_SIZE + OFFSET * 2, 0, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, 0);
}

void handleMenuSelection() {
    menuOpen = !menuOpen;
    if (menuOpen) {
        // 显示菜单
        setfillcolor(RED);
        solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, SIZE * GRID_SIZE / 2 + 120, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, SIZE * GRID_SIZE / 2 + 260);
        settextstyle(32, 0, _T("宋体"));
        settextcolor(GREEN);
        outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 40, SIZE * GRID_SIZE / 2 + 130, _T("单人对战"));
        outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 40, SIZE * GRID_SIZE / 2 + 170, _T("双人对战"));
        outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 40, SIZE * GRID_SIZE / 2 + 210, _T("返回菜单"));
    }
    else {
        // 如果菜单已经关闭，就清除菜单项
        role = -role; // 为了排除一个奇奇怪怪的 bug,去了之后会换角色
        setfillcolor(LIGHTRED);
        solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, SIZE * GRID_SIZE / 2 + 120, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, SIZE * GRID_SIZE / 2 + 300);
        return;
    }

    MOUSEMSG msg;
    while (true) {
        // 获取鼠标事件
        msg = GetMouseMsg();

        switch (msg.uMsg) {
        case WM_LBUTTONDOWN:
            if (msg.x >= SIZE * GRID_SIZE + OFFSET * 2 + 20 && msg.x <= SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20) {
                if (menuOpen) {
                    // 如果菜单已经打开，就处理菜单项的点击事件
                    if (msg.y >= SIZE * GRID_SIZE / 2 + 120 && msg.y < SIZE * GRID_SIZE / 2 + 160) {
                        // 单人对战
                        startSinglePlayerBattle();
                        return;
                    }
                    else if (msg.y >= SIZE * GRID_SIZE / 2 + 160 && msg.y < SIZE * GRID_SIZE / 2 + 200) {
                        // 双人对战
                        startDoublePlayerBattle();
                        return;
                    }
                    else if (msg.y >= SIZE * GRID_SIZE / 2 + 200 && msg.y < SIZE * GRID_SIZE / 2 + 240) {
                        // 返回主菜单
                        initializeGame();
                        return;
                    }
                    else if (msg.y >= SIZE * GRID_SIZE / 2 + 60 && msg.y <= SIZE * GRID_SIZE / 2 + 110) {
                        // 点击菜单按钮，收起菜单
                        handleMenuSelection();
                        return;
                    }
                }
            }
            break;
        }
    }
}






void undo(void) {
    if (!history.empty()) {
        std::pair<int, int> last_move = history.top();
        history.pop();
        board[last_move.first][last_move.second] = 0;
        //role = -role;  // Switch roles
    }
}
