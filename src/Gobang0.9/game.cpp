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
        if (check_key()) {
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

void startSinglePlayerBattle() {
    // ...
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