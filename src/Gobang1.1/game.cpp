// game.cpp
#include "game.h"
#include "board.h"
#include "input.h"

void showResult(const TCHAR* resultText) {
    show_board();
    draw_sidebar();
    setfillcolor(RED);
    solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, 20, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, 70);
    settextstyle(32, 0, _T("����"));
    settextcolor(GREEN);
    outtextxy(SIZE * GRID_SIZE + OFFSET * 2 + 30, 30, resultText);
    _getch();
    closegraph();
}


void startDoublePlayerBattle() {
    initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
    init_board();
    while (1) {
        show_board();
        draw_sidebar();
        get_key();
        std::pair<bool, std::pair<int, int>> result = check_key(5);
        if (result.first) {
            showResult(role == 1 ? _T("����ʤ��!") : _T("����ʤ��!"));
            return;
        }
        if (isBoardFull()) {
            showResult(_T("ƽ�֣�"));
            return;
        }
        role = -role;
    }
}


bool isBoardFull() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

void computerMove() {
    if (isBoardFull()) {
        return;
    }

    int bestScore = -INFINITY;
    std::pair<int, int> move;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                // ��������������
                board[i][j] = -1;
                int score = minimax(2, -999999, 999999, true);
                // ��������
                board[i][j] = 0;
                if (score > bestScore) {
                    bestScore = score;
                    move = std::make_pair(i, j);
                }
            }
        }
    }

    // �����ŵ�λ������
    board[move.first][move.second] = -1;
    key_x = move.first;
    key_y = move.second;
    history.push(move);
}



double position_weight(int x, int y) {
    return weights[x][y];
}

int evaluate_position(int x, int y) {
    int score = 0;
    double weight = position_weight(x, y);

    // �������λ�õ��ĸ�����
    for (int i = 1; i <= 4; i++) {
        std::pair<bool, std::pair<int, int>> result = check_key(i);
        if (result.first) {
            if (i == 4) {
                score += 10000 * weight;
            }
            else if (i == 3) {
                score += 1000 * weight;
            }
            else if (i == 2) {
                score += 100 * weight;
            }
            else if (i == 1) {
                score += 10 * weight;
            }
        }
    }

    return score;
}

int evaluate() {
    int score = 0;

    // ���������ϵ�ÿ��λ��
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == -1) {
                // ������λ���ǵ��Ե����ӣ����ӷ���
                score += evaluate_position(i, j);
            }
            else if (board[i][j] == 1) {
                // ������λ������ҵ����ӣ����ٷ���
                score -= evaluate_position(i, j);
            }
        }
    }

    return score;
}

int minimax(int depth, int alpha, int beta, bool isMaximizingPlayer) {
    if (depth == 0) {
        return evaluate();
    }

    if (isMaximizingPlayer) {
        int maxEval = -999999;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == 0) {
                    // ��������������
                    board[i][j] = -1;
                    int eval = minimax(depth - 1, alpha, beta, false);
                    // ��������
                    board[i][j] = 0;
                    maxEval = maxEval > eval ? maxEval : eval;
                    alpha = alpha > eval ? alpha : eval; 
                    if (beta <= alpha) { 
                        return maxEval;
                    }
                }
            }
        }
        return maxEval;
    }
    else {
        int minEval = 999999;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == 0) {
                    // ��������������
                    board[i][j] = 1;
                    int eval = minimax(depth - 1, alpha, beta, true);
                    // ��������
                    board[i][j] = 0;
                    minEval = minEval > eval ? eval : minEval;
                    beta = beta > eval ? eval : beta; 
                    if (beta <= alpha) { 
                        return minEval;
                    }
                }
            }
        }
        return minEval;
    }
}


void startSinglePlayerBattle() {
    initgraph(SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH, SIZE * GRID_SIZE + OFFSET * 2);
    init_board();
    precompute_weights();
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
            showResult(role == 1 ? _T("����ʤ��!") : _T("����ʤ��!"));
            return;
        }
        if (isBoardFull()) {
            showResult(_T("ƽ�֣�"));
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