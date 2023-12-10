// input.h
#pragma once
#include <graphics.h>
#include <easyx.h>
#include <conio.h>
#include <stdio.h>
#include <stack>

extern bool isSinglePlayerModeSelected;
extern bool isColorSelected; 

void drawBlackAndWhiteChess();
void removeBlackAndWhiteChess();
void processMouseClick(ExMessage msg);
void processKeyboardInput();
void get_key(void);

