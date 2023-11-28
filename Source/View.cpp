#include "data.h"

void FixConsoleWindow() {
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void TextColor(int x) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);
}

int GetCurrentColor() {
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(console, &consoleInfo);
	WORD textAttributes = consoleInfo.wAttributes;
	return textAttributes;
}

void SetConsoleBlank() {
	int h = HEIGHT, w = WIDTH;
	TextColor(WHITE);
	for (int i = 0; i < h; i++) {
		GotoXY(0, i);
		for (int j = 0; j < w; j++)
			cout << SPACE;
	}
	GotoXY(0, 0);
}

void HideCursor(bool ok) {
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = !ok;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void ChangeFont(int x)
{
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = x;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_BOLD;
	wcscpy_s(cfi.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

void SetConsoleSize(int w, int h) {
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, r.left, r.top, w, h, TRUE); // 800 width, 100 height
}

void DrawBox(int w, int h, int x, int y, int color, int Time) {
	int tmp = GetCurrentColor();
	TextColor(color);
	for (int i = 0; i < w / 2; i++) {
		GotoXY(x + w / 2 - i, y);
		cout << H_LINE;
		GotoXY(x + w / 2 + i, y);
		cout << H_LINE;
		Sleep(Time);
	}
	GotoXY(x, y);
	cout << TOP_LEFT;
	GotoXY(x + w - 1, y);
	cout << TOP_RIGHT;
	for (int i = 1; i < h - 1; i++) {
		GotoXY(x, y + i);
		cout << V_LINE;
		for (int j = 1; j < w - 1; j++)
			cout << SPACE;
		cout << V_LINE;
		Sleep(Time);
	}
	GotoXY(x, y + h - 1);
	cout << BOTTOM_LEFT;
	GotoXY(x + w - 1, y + h - 1);
	cout << BOTTOM_RIGHT;
	for (int i = w / 2 - 1; i >= 0; i--) {
		GotoXY(x + w / 2 - i, y + h - 1);
		cout << H_LINE;
		GotoXY(x + w / 2 + i - (w % 2 == 0), y + h - 1);
		cout << H_LINE;
		Sleep(Time);
	}
	TextColor(tmp);
}


void MailContent() {
	TextColor(BLACK);
	GotoXY(63, 0);
	cout << "From: <sender>";
	GotoXY(69, 1);
	cout << "<mail sender>";
	GotoXY(63, 2);
	cout << "To: <receiver>";
	GotoXY(67, 3);
	cout << "<mail receiver>";
	for (int i = 0; i < 57; i++) {
		GotoXY(62 + i, 4);
		cout << H_LINE;
		GotoXY(62 + i, 6);
		cout << H_LINE;
		GotoXY(62 + i, 26);
		cout << H_LINE;
	}
	GotoXY(62, 5);
	cout << "<subject>";
	GotoXY(62, 7);
	cout << "...";

	GotoXY(62, 27);
	cout << "*1 attachment. Enter path to save: ";
	DrawBox(57, 3, 62, 28, GRAY, 0);
	GotoXY(63, 29);
	cout << " > ";

}
void MainMenu() {
	SetConsoleBlank();
	int tmp = GetCurrentColor();
	TextColor(BLACK);
	for (int i = 0; i < HEIGHT; i++) {
		GotoXY(60, i);
		cout << V_LINE;

		GotoXY(20, i);
		cout << V_LINE;
	}

	DrawBox(16, 3, 2, 0, CYAN, 0);
	GotoXY(3, 1);
	TextColor(CYAN);
	cout << "+ New message";

	string mail = "test123@gmail.com";
	GotoXY(2, 4);
	TextColor(BLUE);
	cout << mail;
	GotoXY(5, 6);
	TextColor(RED);
	cout << "Sent";
	GotoXY(5, 8);
	TextColor(BLUE);
	cout << "Inbox";
	GotoXY(5, 10);
	cout << "Project";
	GotoXY(5, 12);
	cout << "Important";
	GotoXY(5, 14);
	cout << "Spam";
	GotoXY(5, 16);
	cout << "Work";

	TextColor(YELLOW);
	GotoXY(3, 6);
	cout << L_TRIANGLE;

	DrawBox(37, 4, 22, 0, RED, 0);
	GotoXY(23, 1);
	TextColor(RED);
	cout << "<sender>";
	GotoXY(23, 2);
	cout << "<subject>";

	DrawBox(37, 4, 22, 4, CYAN, 0);
	GotoXY(23, 5);
	TextColor(CYAN);
	cout << "<sender>";
	GotoXY(23, 6);
	cout << "<subject>";
	DrawBox(37, 4, 22, 8, CYAN, 0);
	GotoXY(23, 9);
	TextColor(CYAN);
	cout << "<sender>";
	GotoXY(23, 10);
	cout << "<subject>";
	DrawBox(37, 4, 22, 12, CYAN, 0);
	GotoXY(23, 13);
	TextColor(CYAN);
	cout << "<sender>";
	GotoXY(23, 14);
	cout << "<subject>";

	MailContent();
	TextColor(tmp);
	if (_getch())
		exit(0);
}