#pragma once
#include "Data.h"

void FixConsoleWindow();
void SetConsoleBlank();
void TextColor(int x);
void HideCursor(bool ok);
void SetConsoleSize(int w, int h);
void MainMenu(CONFIG& cnf);
int GetCurrentColor();
void FontSize(int x);