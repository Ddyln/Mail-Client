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
void ClearBorder(int w, int h, int x, int y);
void DrawBorder(int w, int h, int x, int y, int col);
void PrintTextInLine(string text, int w, int x, int y, int color = BLACK);
bool EnterTextInTable(int len, vector <string>& table, int row);
void EnterMail(CONFIG& cnf);
string FileName(string& s);
void DisplayFileNotFound();
void DisplayFileSizeOverflow();