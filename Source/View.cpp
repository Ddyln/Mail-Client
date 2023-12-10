#include "Data.h"
#include "View.h"
#include "Control.h"
#include "Model.h"

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

void SetConsoleSize(int w, int h) {
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r); //stores the console's current dimensions
	MoveWindow(console, r.left, r.top, w, h, TRUE); // 800 width, 100 height
}

void FontSize(int x) {
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

void ClearBox(int w, int h, int x, int y) {
	int tmp = GetCurrentColor();
	TextColor(CYAN);
	for (int i = 0; i < h; i++) {
		GotoXY(x, y + i);
		for (int j = 0; j < w; j++)
			cout << ' ';
	}
	TextColor(tmp);
}

void MailContent(MAIL mail, int page) {
	TextColor(BLACK);
	PrintTextInLine("From: " + mail.from, 56, 63, 0, BLACK);
	PrintTextInLine("To: " + mail.to, 56, 63, 1, BLACK);
	PrintTextInLine("Cc: " + mail.cc, 56, 63, 2, BLACK);
	for (int i = 0; i < 57; i++) {
		GotoXY(62 + i, 3);
		cout << H_LINE;
		GotoXY(62 + i, 5);
		cout << H_LINE;
		GotoXY(62 + i, 26);
		cout << H_LINE;
	}
	for (int i = 20; i < 37; i++) {
		GotoXY(62 + i, 24);
		cout << H_LINE;
	}
	string nPage = to_string(max(1, (int)ceil(1.0 * mail.line.size() / 18)));
	while (nPage.size() < 2) nPage = "0" + nPage;
	string curPage = to_string(page + 1);
	while (curPage.size() < 2) curPage = "0" + curPage;
	string footer = "<" + curPage + "/" + nPage + ">";
	GotoXY(62 + 57 / 2 - footer.size() / 2, 25);
	cout << footer;
	PrintTextInLine("Subject: " + mail.subject, 57, 62, 4, BLACK);
	for (int i = 0; i < 18; i++) {
		int curLine = page * 18 + i;
		if (curLine >= mail.line.size())
			break;
		GotoXY(62, 6 + i);
		cout << mail.line[curLine];
	}

	GotoXY(62, 27);
	cout << "*" << mail.att.size() << " attachment. Enter path to save all: ";
	DrawBox(57, 3, 62, 28, GRAY, 0);
	GotoXY(63, 29);
	cout << " > ";

}

void UnhoverButton(int pos) {
	pos = (pos + 1) % 7;
	int tmp = GetCurrentColor();
	if (pos == 0) {
		DrawBox(16, 3, 2, 0, CYAN, 0);
		GotoXY(3, 1);
		TextColor(CYAN);
		cout << "+ New message";
	}
	else {
		string button[6] = { "Sent", "Inbox", "Project", "Important", "Spam", "Work" };
		GotoXY(5, 6 + (pos - 1) * 2);
		TextColor(BLUE);
		cout << button[pos - 1];
		GotoXY(3, 6 + (pos - 1) * 2);
		cout << " ";
	}
	TextColor(tmp);
}

void HoverButton(int pos) {
	pos = (pos + 1) % 7;
	int tmp = GetCurrentColor();
	if (pos == 0) {
		DrawBox(16, 3, 2, 0, RED, 0);
		GotoXY(3, 1);
		TextColor(RED);
		cout << "+ New message";
	}
	else {
		string button[6] = { "Sent", "Inbox", "Project", "Important", "Spam", "Work" };
		GotoXY(5, 6 + (pos - 1) * 2);
		TextColor(RED);
		cout << button[pos - 1];
		TextColor(YELLOW);
		GotoXY(3, 6 + (pos - 1) * 2);
		cout << L_TRIANGLE;
	}
	TextColor(tmp);
}

void PrintTextInLine(string text, int w, int x, int y, int color) {
	GotoXY(x, y);
	int tmp = GetCurrentColor();
	TextColor(color);
	if (text.size() <= w)
		cout << text;
	else
		cout << text.substr(0, w - 3) << "...";
	TextColor(tmp);
}

void HoverMailBox(MAIL mail, int line) {
	DrawBox(37, 4, 22, line * 4, RED, 0);
	PrintTextInLine("From: " + mail.from, 35, 23, 1 + line * 4, RED);
	PrintTextInLine("Subject: " + mail.subject, 35, 23, 2 + line * 4, RED);
}

void UnhoverMailBox(MAIL mail, int line) {
	int col = (mail.read ? BLUE : CYAN);
	DrawBox(37, 4, 22, line * 4, col, 0);
	PrintTextInLine("From: " + mail.from, 35, 23, 1 + line * 4, col);
	PrintTextInLine("Subject: " + mail.subject, 35, 23, 2 + line * 4, col);
}

void MainMenu(LIST& mail, CONFIG& cnf) {
	SetConsoleBlank();
	int tmp = GetCurrentColor();
	TextColor(BLACK);
	for (int i = 0; i < HEIGHT; i++) {
		GotoXY(60, i);
		cout << V_LINE;

		GotoXY(20, i);
		cout << V_LINE;
	}

	DrawBox(16, 3, 2, 0, RED, 0);
	GotoXY(3, 1);
	TextColor(RED);
	cout << "+ New message";

	GotoXY(2, 4);
	TextColor(BLUE);
	if (cnf.mail.size() > 20) {
		GotoXY(0, 4);
		for (int i = 0; i < 17; i++)
			cout << cnf.mail[i];
		cout << "...";
	}
	else {
		GotoXY(10 - cnf.mail.size() / 2, 4);
		cout << cnf.mail;
	}

	GotoXY(5, 6);
	TextColor(BLUE);
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
	int pos = 6;
	
	while (true) {
		unsigned char c = toupper(_getch());
		if (c == DOWN_ARROW || c == UP_ARROW) {
			int newPos = pos;
			if (c == DOWN_ARROW)
				newPos = (pos + 1) % 7;
			else
				newPos = (pos - 1 + 7) % 7;
			UnhoverButton(pos);
			HoverButton(newPos);
			pos = newPos;
		}
		else if (c == ENTER) {
			if (pos == 6) {

			}
			else {
				vector <int> tmp;
				for (int i = 0; i < mail.size(); i++) {
					if (mail[i].type == pos)
						tmp.push_back(i);
				}
				int curPage = 0, curLine = 0;
				int nPage = (int)ceil(1.0 * tmp.size() / 7), nLine = min(7, tmp.size());
				for (int i = 1; i < nLine; i++)
					UnhoverMailBox(mail[tmp[i]], i);
				if (nLine)
					HoverMailBox(mail[tmp[0]], 0);

				while (true) {
					int nCurPage = curPage, nCurLine = curLine;
					string s = to_string(curPage + 1);
					while (s.size() < 2) s = "0" + s;
					string footer = "<" + s;
					s = to_string(max(1, nPage));
					while (s.size() < 2) s = "0" + s;
					footer += "/" + s + ">";
					GotoXY(20 + 17, HEIGHT - 2);
					cout << footer;
					GotoXY(20 + 15, HEIGHT - 3);
					for (int i = 0; i < 11; i++)
						cout << H_LINE;
					unsigned char c = toupper(_getch());
					if (c == ESC) {
						if (tmp.size())
							UnhoverMailBox(mail[tmp[nCurPage * 7 + curLine]], curLine);
						break;
					}
					if (tmp.size() == 0)
						continue;
					else if (c == ENTER) {
						int page = 0, nPage = max(1, (int)ceil(1.0 * mail[tmp[curPage * 7 + curLine]].line.size() / 18));
						mail[curPage * 7 + curLine].markAsRead(cnf);
						MailContent(mail[curPage * 7 + curLine], page);
						while (true) {
							bool main = 1;
							unsigned char c = toupper(_getch());
							if (c == ESC) {
								break;
							}
							else if (c == RIGHT_ARROW)
								page = (page + 1) % nPage;
							else if (c == LEFT_ARROW)
								page = (page - 1 + nPage) % nPage;
							else if (c == DOWN_ARROW)
								main = 0;
							if (!main) {
								GotoXY(66, 29);
								string path = "";
								int tmpCol = GetCurrentColor();
								TextColor(YELLOW);
								EnterPath(path, 50);
								MAIL x = mail[tmp[curPage * 7 + curLine]];
								if (path != "" && x.att.size()) {
									// save file here
									string fileName = "", encodedData = "";
									for (int i = 0; i < x.att.size(); i++) {
										int pos = x.att[i].find("Content-Transfer-Encoding: base64");
										encodedData = x.att[i].substr(Find(x.att[i], '4', pos) + 1);
										pos -= 2;
										int tmpPos = pos;
										while (x.att[i][pos] != '\"') pos--;
										pos++;
										fileName = x.att[i].substr(pos, tmpPos - pos + 1);
										Base64::decode(encodedData, path + "/" + fileName);
									}
								}
								TextColor(tmpCol);
							}
							ClearBox(57, 18, 62, 6);
							MailContent(mail[tmp[curPage * 7 + curLine]], page);
						}
						ClearBox(58, HEIGHT, 61, 0);
					}
					else if (c == RIGHT_ARROW || c == LEFT_ARROW) {
						if (c == RIGHT_ARROW) {
							nCurPage = (curPage + 1) % nPage, nCurLine = 0;
						}
						else if (c == LEFT_ARROW) {
							nCurPage = (curPage - 1 + nPage) % nPage, nCurLine = 0;
						}
						ClearBox(39, HEIGHT, 21, 0);
						for (int i = nCurPage * 7 + 1; i < min((nCurPage + 1) * 7, tmp.size()); i++)
							UnhoverMailBox(mail[tmp[i]], i - nCurPage * 7);
						HoverMailBox(mail[tmp[nCurPage * 7]], 0);
						curPage = nCurPage;
						curLine = nCurLine;
					}
					else {
						if (curPage != nPage - 1)
							nLine = 7;
						else
							nLine = tmp.size() % 7;
						if (c == DOWN_ARROW)
							nCurLine = (curLine + 1) % nLine;
						else if (c == UP_ARROW)
							nCurLine = (curLine - 1 + nLine) % nLine;
						UnhoverMailBox(mail[tmp[nCurPage * 7 + curLine]], curLine);
						HoverMailBox(mail[tmp[nCurPage * 7 + nCurLine]], nCurLine);
					}
					curPage = nCurPage;
					curLine = nCurLine;
				}
				ClearBox(39, HEIGHT, 21, 0);
			}
		}
	}

}