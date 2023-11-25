// game.cpp
#include "game.h"
#include "board.h"
#include "input.h"

void showResult(const TCHAR* resultText) {
    show_board();
    draw_sidebar();
    setfillcolor(RED);
    solidrectangle(SIZE * GRID_SIZE + OFFSET * 2 + 20, 20, SIZE * GRID_SIZE + OFFSET * 2 + SIDEBAR_WIDTH - 20, 70);
    settextstyle(32, 0, _T("宋体"));
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
            showResult(role == 1 ? _T("黑子胜利!") : _T("白子胜利!"));
            return;
        }
        if (isBoardFull()) {
            showResult(_T("平局！"));
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
                // 尝试在这里落子
                board[i][j] = -1;
                int score = minimax(2, -999999, 999999, true);
                // 撤销尝试
                board[i][j] = 0;
                if (score > bestScore) {
                    bestScore = score;
                    move = std::make_pair(i, j);
                }
            }
        }
    }

    // 在最优的位置落子
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

    // 遍历这个位置的四个方向
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

    // 遍历棋盘上的每个位置
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == -1) {
                // 如果这个位置是电脑的棋子，增加分数
                score += evaluate_position(i, j);
            }
            else if (board[i][j] == 1) {
                // 如果这个位置是玩家的棋子，减少分数
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
                    // 尝试在这里落子
                    board[i][j] = -1;
                    int eval = minimax(depth - 1, alpha, beta, false);
                    // 撤销尝试
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
                    // 尝试在这里落子
                    board[i][j] = 1;
                    int eval = minimax(depth - 1, alpha, beta, true);
                    // 撤销尝试
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
            // 玩家落子
            get_key();
        }
        else {
            // 电脑落子
            computerMove();
        }
        std::pair<bool, std::pair<int, int>> result = check_key(5);
        if (result.first) {
            showResult(role == 1 ? _T("黑子胜利!") : _T("白子胜利!"));
            return;
        }
        if (isBoardFull()) {
            showResult(_T("平局！"));
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