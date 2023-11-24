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
            settextstyle(32, 0, _T("����"));
            outtextxy(SIZE * GRID_SIZE / 2, SIZE * GRID_SIZE / 2, role == 1 ? _T("���ӷ�ʤ����") : _T("���ӷ�ʤ����"));
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

    // ��ȡ������µ�����λ��
    int last_x = history.top().first;
    int last_y = history.top().second;

    // �������������ӵ���Χ�˸�����
    for (int k = 0; k < 8; k++) {
        int x = last_x + dx[k];
        int y = last_y + dy[k];

        // ����Ƿ��������ڣ���δ����
        if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y] == 0) {
            // ��������������
            board[x][y] = -1;

            // �������Ƿ�����������
            std::pair<bool, std::pair<int, int>> result = check_key(4);
            if (result.first) {
                // ����У���������������ֹ
                key_x = result.second.first;
                key_y = result.second.second;
                history.push(std::make_pair(key_x, key_y));
                return;
            }

            // ��������
            board[x][y] = 0;
        }
    }

    // ���û����Ҫ��ֹ���������ߣ�������ҵ�������Χ�������
    while (true) {
        int i = rand() % BOARD_SIZE;
        int j = rand() % BOARD_SIZE;

        // ����Ƿ�����ҵ�������Χ
        for (int k = 0; k < 8; k++) {
            int x = i + dx[k];
            int y = j + dy[k];
            if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE && board[x][y] == 1) {
                // �����ѡλ���Ƿ�Ϊ��
                if (board[i][j] == 0) {
                    // ��������������
                    board[i][j] = -1;

                    // �������Ƿ�����������
                    std::pair<bool, std::pair<int, int>> result = check_key(3);
                    if (result.first) {
                        // ����У���������������ֹ
                        key_x = result.second.first;
                        key_y = result.second.second;
                        history.push(std::make_pair(key_x, key_y));
                        return;
                    }
                    else {
                        // ���û�У������������
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
            // �������
            get_key();
        }
        else {
            // ��������
            computerMove();
        }
        std::pair<bool, std::pair<int, int>> result = check_key(5);
        if (result.first) {
            show_board();
            settextstyle(32, 0, _T("����"));
            outtextxy(SIZE * GRID_SIZE / 2, SIZE * GRID_SIZE / 2, role == 1 ? _T("���ӷ�ʤ����") : _T("���ӷ�ʤ����"));
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
    settextstyle(30, 0, _T("����"));
    outtextxy(260, 160, _T("˫�˶�ս"));
    outtextxy(260, 260, _T("�˻���ս"));
    outtextxy(260, 360, _T("�˳���Ϸ"));
}