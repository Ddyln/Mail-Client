#pragma once
#include "Data.h"

void FixConsoleWindow();
void SetConsoleBlank();
void TextColor(int x);
void HideCursor(bool ok);
void SetConsoleSize(int w, int h);
void MainMenu(LIST& MAIL, CONFIG& cnf);
int GetCurrentColor();
void FontSize(int x);
void MailContent(MAIL mail, int page = 0);
void DrawBox(int w, int h, int x, int y, int color, int Time);
void ClearBox(int w, int h, int x, int y);
void PrintTextInLine(string text, int w, int x, int y, int color = BLACK);