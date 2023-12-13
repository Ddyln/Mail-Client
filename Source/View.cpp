#include "Data.h"
#include "View.h"
#include "Control.h"
#include "Model.h"
static char recvbuf[buflen] = { 0 };

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

void ClearBorder(int w, int h, int x, int y) {
	int tmp = GetCurrentColor();
	TextColor(CYAN);
	for (int i = 0; i < w; i++) {
		GotoXY(x + i, y);
		cout << ' ';
		GotoXY(x + i, y + h - 1);
		cout << ' ';
	}
	for (int i = 1; i < h - 1; i++) {
		GotoXY(x, y + i);
		cout << ' ';
		GotoXY(x + w - 1, y + i);
		cout << ' ';
	}
	TextColor(tmp);
}

void DrawBorder(int w, int h, int x, int y, int col) {
	int tmp = GetCurrentColor();
	TextColor(col);
	for (int i = 0; i < w; i++) {
		GotoXY(x + i, y);
		cout << H_LINE;
		GotoXY(x + i, y + h - 1);
		cout << H_LINE;
	}

	for (int i = 1; i < h - 1; i++) {
		GotoXY(x, y + i);
		cout << V_LINE;
		GotoXY(x + w - 1, y + i);
		cout << V_LINE;
	}
	GotoXY(x, y); cout << TOP_LEFT;
	GotoXY(x + w - 1, y); cout << TOP_RIGHT;
	GotoXY(x, y + h - 1); cout << BOTTOM_LEFT;
	GotoXY(x + w - 1, y + h - 1); cout << BOTTOM_RIGHT;
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
	pos = (pos + 1) % 6;
	int tmp = GetCurrentColor();
	if (pos == 0) {
		DrawBox(16, 3, 2, 1, CYAN, 0);
		GotoXY(3, 2);
		TextColor(CYAN);
		cout << "+ New message";
	}
	else {
		string button[5] = { "Inbox", "Project", "Important", "Spam", "Work" };
		GotoXY(5, 9 + (pos - 1) * 3);
		TextColor(BLUE);
		cout << button[pos - 1];
		GotoXY(3, 9 + (pos - 1) * 3);
		cout << " ";
	}
	TextColor(tmp);
}

void HoverButton(int pos) {
	pos = (pos + 1) % 6;
	int tmp = GetCurrentColor();
	if (pos == 0) {
		DrawBox(16, 3, 2, 1, RED, 0);
		GotoXY(3, 2);
		TextColor(RED);
		cout << "+ New message";
	}
	else {
		string button[5] = { "Inbox", "Project", "Important", "Spam", "Work" };
		GotoXY(5, 9 + (pos - 1) * 3);
		TextColor(RED);
		cout << button[pos - 1];
		TextColor(YELLOW);
		GotoXY(3, 9 + (pos - 1) * 3);
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

void UnhoverTitle(int i) {
	if (i < 5)
		ClearBorder(70, (i == 4 ? 19 : 3), 26, 2 * i);
	int tmp = GetCurrentColor();
	TextColor(tmp);
	if (i == 4) {
		for (int j = 1; j < 69; j++) {
			GotoXY(26 + j, 26);
			cout << H_LINE;
			GotoXY(26 + j, 8);
			cout << H_LINE;
		}
	}
	else if (i == 5) {
		DrawBorder(70, 3, 26, 28, BLACK);
	}
	else {
		for (int j = 1; j < 69; j++) {
			GotoXY(26 + j, i * 2);
			cout << H_LINE;
			GotoXY(26 + j, 2 + i * 2);
			cout << H_LINE;
		}
	}
	TextColor(tmp);
}

void HoverTitle(int i) {
	if (i < 4)
		DrawBorder(70, 3, 26, 2 * i, RED);
	else if (i == 4)
		DrawBorder(70, 19, 26, 2 * i, RED);
	else
		DrawBorder(70, 3, 26, 28, RED);
}

void UnhoverFunc(int i) {
	int tmp = GetCurrentColor();
	TextColor(BLACK);
	string txt[2] = { " Send", "Cancel" };
	DrawBox(8, 3, 107, 10 + 9 * i, BLACK, 0);
	GotoXY(108, 11 + 9 * i);
	cout << txt[i];
	TextColor(tmp);
}

void HoverFunc(int i) {
	int tmp = GetCurrentColor();
	TextColor(RED);
	string txt[2] = { " Send", "Cancel" };
	DrawBox(8, 3, 107, 10 + 9 * i, RED, 0);
	GotoXY(108, 11 + 9 * i);
	cout << txt[i];
	TextColor(tmp);
}

bool EnterTextInTable(int len, vector <string>& table, int row) {
	unsigned char c;
	HideCursor(0);
	if (table.empty()) table.push_back("");
	int oriX = WhereX() - (int)table.back().size(), oriY = WhereY() - (int)table.size() + 1, page = 1;
	int cnt = 0;
	while (true) {
		int tmp1 = WhereX(), tmp2 = WhereY();
		GotoXY(59, 25);
		cout << "<" << (page < 10 ? "0" : "") << page << ">";
		GotoXY(tmp1, tmp2);
		c = _getch();
		if (c == ENTER) {
			if ((int)table.size() % row == 0) {
				page++;
				ClearBox(68, 15, 27, 9);
				GotoXY(oriX, oriY);
			}
			else
				GotoXY(oriX, WhereY() + 1);
			table.push_back("");
		}
		else if (c == TAB)
			break;
		else if (c == ESC) {
			table.clear();
			ClearBox(68, 15, 27, 9);
			GotoXY(59, 25);
			cout << "<01>";
			GotoXY(oriX, oriY);
			HideCursor(1);
			return 0;
		}
		else if (c == 224)
			c = _getch();
		else if (c == BACK_SPACE) {
			if (table.size() == 1 && table.back().empty()) {
				continue;
			}
			if (table.back().empty()) {
				if (table.size() % row == 1) {
					if (page == 1)
						continue;
					else {
						page--;
						table.pop_back();
						if (table.back().size()) table.back().pop_back();
						ClearBox(68, 15, 27, 9);
						for (int i = (page - 1) * row; i < page * row; i++) {
							GotoXY(oriX, oriY + i - (page - 1) * row);
							cout << table[i];
						}
					}
				}
				else {
					table.pop_back();
					if (table.back().size()) table.back().pop_back();
					GotoXY(oriX + (int)table.back().size(), WhereY() - 1);
					cout << ' ';
					GotoXY(WhereX() - 1, WhereY());
				}
			}
			else {
				table.back().pop_back();
				GotoXY(WhereX() - 1, WhereY());
				cout << ' ';
				GotoXY(WhereX() - 1, WhereY());
			}
		}
		else {
			cout << c;
			table.back() += c;
			if (table.back().size() == len) {
				if ((int)table.size() % row == 0) {
					page++;
					ClearBox(68, 15, 27, 9);
					GotoXY(oriX, oriY);
				}
				else
					GotoXY(oriX, WhereY() + 1);
				table.push_back("");
			}
		}
	}
	HideCursor(1);
	return 1;
}

void EnterMail(CONFIG& cnf) {
	GotoXY(27, 1);
	cout << "  To   " << V_LINE << " ";
	GotoXY(27, 3);
	cout << "  Cc   " << V_LINE << " ";
	GotoXY(27, 5);
	cout << "  Bcc  " << V_LINE << " ";
	GotoXY(27, 7);
	cout << "Subject" << V_LINE << " ";
	GotoXY(27, 27);
	cout << " * Number of attachments: 0";
	GotoXY(28, 29);
	cout << " > ";
	for (int i = 0; i < 2; i++)
		UnhoverFunc(i);
	for (int i = 1; i < 6; i++)
		UnhoverTitle(i);
	for (int i = 1; i < HEIGHT - 1; i++) {
		GotoXY(101, i);
		cout << V_LINE;
	}
	for (int i = 0; i < 20; i++) {
		GotoXY(51 + i, 24);
		cout << H_LINE;
	}
	GotoXY(59, 25);
	cout << "<01>";
	HoverTitle(0);
	vector <pair <int, int>> buttonPos = { {36, 1}, {36, 3}, {36, 5}, {36, 7}, {28, 9}, {31, 29} };
	int len[] = { 59, 59, 59, 59, 67, 64 };
	string buttonText[6] = { "" };
	int n = 6;
	vector <string> table;
	int pos = 0, col = 0, totalFileSize = 0;
	vector <string> att;
	while (true) {
		unsigned char c = _getch();
		if (c == ESC)
			return;
		if (col == 0) {
			if (c == 224) {
				c = toupper(_getch());
				int nPos = pos;
				if (c == DOWN_ARROW || c == UP_ARROW) {
					if (c == DOWN_ARROW)
						nPos = min(5, pos + 1);
					else 
						nPos = max(0, pos - 1);
					UnhoverTitle(pos);
					HoverTitle(nPos);
					pos = nPos;
				}
				else if (c == RIGHT_ARROW) {
					col = 1;
					pos = 0;
					for (int i = 0; i < 6; i++)
						UnhoverTitle(i);
					HoverFunc(pos);
				}
			}
			else if (c == ENTER) {
				int& x = buttonPos[pos].first, & y = buttonPos[pos].second;
				GotoXY(x, y);
				if (pos != 4 && pos != 5)
					EnterText(buttonText[pos], len[pos]);
				else if (pos == 4)
					EnterTextInTable(len[pos], table, 15);
				else if (pos == 5) {
					while (true) {
						string& file = buttonText[pos];
						file = "";
						GotoXY(31, 29);
						for (int i = 31; i < 95; i++)
							cout << ' ';
						GotoXY(31, 29);
						if (!EnterText(file, len[pos])) break;
						GotoXY(31, 29);
						for (int i = 31; i < 95; i++)
							cout << ' ';
						GotoXY(31, 29);
						int fsz = FileSize(file);
						if (fsz < 0) {
							int tmp = GetCurrentColor();
							TextColor(RED);
							cout << "File not Found!";
							TextColor(tmp);
							_getch();
							continue;
						}
						if (fsz + totalFileSize > 3000000) {
							int tmp = GetCurrentColor();
							TextColor(RED);
							cout << "Total files' size cannot exceed 3MB";
							TextColor(tmp);
							_getch();
							continue;
						}
						totalFileSize += fsz;
						att.push_back(file);
						GotoXY(53, 27);
						cout << att.size();
					}
				}
				x = WhereX(), y = WhereY();
			}
		}
		else {
			if (c == 224) {
				c = toupper(_getch());
				int nPos = 0;
				if (c == DOWN_ARROW || c == UP_ARROW) {
					if (c == DOWN_ARROW)
						nPos = min(1, pos + 1);
					else
						nPos = max(0, pos - 1);
					UnhoverFunc(pos);
					HoverFunc(nPos);
					pos = nPos;
				}
				else if (c == LEFT_ARROW) {
					col = 0;
					pos = 0;
					for (int i = 0; i < 2; i++)
						UnhoverFunc(i);
					HoverTitle(pos);
				}
			}
			else if (c == ENTER) {
				if (pos == 1) 
					return;
				int pos = 0;
				WSADATA wsaData;
				if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
					cerr << "Failed to initialize Winsock\n";
					return ;
				}

				SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
				if (sock == INVALID_SOCKET) {
					cerr << "Failed to create socket\n";
					WSACleanup();
					return;
				}

				sockaddr_in server;
				server.sin_addr.s_addr = inet_addr(cnf.server.c_str());
				server.sin_family = AF_INET;
				server.sin_port = htons(cnf.SMTP);
				connect(sock, (sockaddr*)&server, sizeof(server));
				Recv(sock, recvbuf);
				string command = "EHLO [" + cnf.server + "]\n";
				Send(sock, command);
				Recv(sock, recvbuf);
				command = "MAIL FROM:<" + cnf.mail + ">\n";
				Send(sock, command);
				Recv(sock, recvbuf);
				for (int i = 0; i < 3; i++) {
					int pos = 0;
					string& s = buttonText[i];
					if (s.empty()) continue;
					if (s.back() != ',') s.push_back(',');
					while (true) {
						int oldPos = pos;
						pos = Find(s, ',', pos);
						string receiver = (s.substr(oldPos, pos - oldPos));
						command = "RCPT TO:<" + receiver + ">\n";
						Send(sock, command);
						Recv(sock, recvbuf);
						pos++;
						pos = FindNot(s, ' ', pos);
						if (pos < 0) break;
					}
				}
				command = "DATA\n";
				Send(sock, command);
				Recv(sock, recvbuf);
				command = "From: " + cnf.mail + "\n";
				Send(sock, command);
				command = "To: " + buttonText[0] + "\n";
				Send(sock, command);
				if (buttonText[1].size()) {
					command = "Cc: " + buttonText[1] + "\n";
					Send(sock, command);
				}
				if (buttonText[3].size()) {
					command = "Subject: " + buttonText[3] + "\n";
					Send(sock, command);
				}
				if (att.size()) {
					command = "MIME-Version: 1.0\n";
					Send(sock, command);
					command = "Content-Type: multipart/mixed; boundary=\"------------OKIXMxsox0yQHMH3fEfrxS50\"\n";
					Send(sock, command);
					command = "\n";
					Send(sock, command);
					command = MIME_start + "\n";
					Send(sock, command);
					command = "------------OKIXMxsox0yQHMH3fEfrxS50\n";
					Send(sock, command);
					command = "Content-Type: text/plain; charset=UTF-8; format=flowed\n";
					Send(sock, command);
					command = "Content-Transfer-Encoding: 7bit\n";
					Send(sock, command);
					command = "\n";
					Send(sock, command);
					command = "";
					for (string& s : table) {
						if (s.size() < len[4]) {
							command += s + "\n";
							Send(sock, command);
							command = "";
						}
						else
							command += s;
					}
					if (command.size())
						Send(sock, command += "\n");
					command = "\n";
					Send(sock, command);
					for (int i = 0; i < att.size(); i++) {
						command = "--------------OKIXMxsox0yQHMH3fEfrxS50\n";
						Send(sock, command);
						command = "Content-Disposition: attachment; filename=\"" + FileName(att[i]) + "\"\n";
						Send(sock, command);
						command = "Content-Transfer-Encoding: base64\n";
						Send(sock, command);
						command = "\n";
						Send(sock, command);
						GotoXY(0, 0);
						command = Base64::encode(att[i]);
						for (int i = 0; i < command.size(); i += 50) {
							string tmp = command.substr(i, min(50, (int)command.size() - i)) + "\n";
							Send(sock, tmp);
						}
					}
					command = "\n";
					Send(sock, command);
					command = "--------------OKIXMxsox0yQHMH3fEfrxS50--\n";
					Send(sock, command);
					command = ".\n";
					Send(sock, command);
				}
				else {
					command = "\n";
					Send(sock, command);
					command = "";
					for (string& s : table) {
						if (s.size() < len[4]) {
							command += s + "\n";
							Send(sock, command);
							command = "";
						}
						else
							command += s;
					}
					command = ".\n";
					Send(sock, command);
				}
				Recv(sock, recvbuf);
				command = "QUIT\n";
				Send(sock, command);
				Recv(sock, recvbuf);
				closesocket(sock);
				WSACleanup();
				return;
			}
		}
	}
}

string FileName(string& s) {
	int i = s.size();
	i--;
	while (s[i] != '/' && s[i] != '\\') i--;
	return s.substr(i + 1);
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

	DrawBox(16, 3, 2, 1, RED, 0);
	GotoXY(3, 2);
	TextColor(RED);
	cout << "+ New message";

	TextColor(YELLOW);
	if (cnf.mail.size() > 20) {
		GotoXY(0, 6);
		for (int i = 0; i < 17; i++)
			cout << cnf.mail[i];
		cout << "...";
	}
	else {
		GotoXY(10 - cnf.mail.size() / 2, 6);
		cout << cnf.mail;
	}
	TextColor(BLACK);
	for (int i = 0; i < 20; i++) {
		GotoXY(i, 7);
		cout << H_LINE;
		GotoXY(i, 5);
		cout << H_LINE;
	}
	GotoXY(20, 7);
	cout << RIGHT_CROSS;
	GotoXY(20, 5);
	cout << RIGHT_CROSS;
	for (int i = 0; i < 5; i++)
		UnhoverButton(i);
	int pos = 5, curMailSize = mail.size();
	bool wait = 0;
	thread mailThread([&mail, &cnf, &wait]() {
		while (true) {
			wait = 1;
			Init(mail, cnf);
			wait = 0;
			this_thread::sleep_for(std::chrono::seconds(cnf.autoload));
		}
	});

	while (true) {
		unsigned char c = toupper(_getch());
		if (c == DOWN_ARROW || c == UP_ARROW) {
			int newPos = pos;
			if (c == DOWN_ARROW)
				newPos = (pos + 1) % 6;
			else
				newPos = (pos - 1 + 6) % 6;
			UnhoverButton(pos);
			HoverButton(newPos);
			pos = newPos;
		}
		else if (c == ENTER) {
			if (pos == 5) {
				ClearBox(WIDTH - 21, HEIGHT, 21, 0);
				EnterMail(cnf);
				ClearBox(WIDTH - 21, HEIGHT, 21, 0);

				for (int i = 0; i < HEIGHT; i++) {
					GotoXY(60, i);
					cout << V_LINE;
				}
			}
			else {
				vector <int> filteredIndex;
				for (int i = 0; i < mail.size(); i++) {
					if (mail[i].type == pos)
						filteredIndex.push_back(i);
				}
				int curPage = 0, curLine = 0;
				int nPage = (int)ceil(1.0 * filteredIndex.size() / 7), nLine = min(7, filteredIndex.size());
				for (int i = 1; i < nLine; i++)
					UnhoverMailBox(mail[filteredIndex[i]], i);
				if (nLine)
					HoverMailBox(mail[filteredIndex[0]], 0);
				
				while (true) {
					if (mail.size() > curMailSize) {
						while (wait);
						filteredIndex.clear();
						for (int i = 0; i < mail.size(); i++) {
							if (mail[i].type == pos)
								filteredIndex.push_back(i);
						}
						nPage = (int)ceil(1.0 * filteredIndex.size() / 7); 
						if (curPage != nPage - 1)
							nLine = 7;
						else {
							nLine = filteredIndex.size() % 7;
							if (!nLine && filteredIndex.size()) nLine = 7;
						}
						for (int i = curPage * 7; i < min((curPage + 1) * 7, filteredIndex.size()); i++)
							UnhoverMailBox(mail[filteredIndex[i]], i - curPage * 7);
						HoverMailBox(mail[filteredIndex[curPage * 7]], curLine);
						curMailSize = mail.size();
					}
					string s = to_string(curPage + 1);
					while (s.size() < 2) s = "0" + s;
					string footer = "<" + s;
					s = to_string(max(1, nPage));
					while (s.size() < 2) s = "0" + s;
					footer += "/" + s + ">";
					GotoXY(20 + 17, HEIGHT - 2);
					TextColor(BLACK);
					cout << footer;
					GotoXY(20 + 15, HEIGHT - 3);
					for (int i = 0; i < 11; i++)
						cout << H_LINE;
					if (_kbhit()) {
						int nCurPage = curPage, nCurLine = curLine;
						unsigned char c = toupper(_getch());
						if (c == ESC) {
							if (filteredIndex.size())
								UnhoverMailBox(mail[filteredIndex[nCurPage * 7 + curLine]], curLine);
							break;
						}
						if (filteredIndex.size() == 0)
							continue;
						else if (c == ENTER) {
							int page = 0, nPage = max(1, (int)ceil(1.0 * mail[filteredIndex[curPage * 7 + curLine]].line.size() / 18));
							mail[filteredIndex[curPage * 7 + curLine]].markAsRead(cnf);
							MailContent(mail[filteredIndex[curPage * 7 + curLine]], page);
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
								else if (c == ENTER)
									main = 0;
								if (!main) {
									GotoXY(66, 29);
									string path = "";
									int tmpCol = GetCurrentColor();
									TextColor(YELLOW);
									EnterText(path, 50);
									string tmp = path;
									path = "";
									for (int i = 0; i < tmp.size(); i++)
										path += (tmp[i] == '/' ? '\\' : tmp[i]);
									MAIL x = mail[filteredIndex[curPage * 7 + curLine]];
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
											Base64::decode(encodedData, path + "\\" + fileName);
										}
									}
									TextColor(tmpCol);
								}
								ClearBox(57, 18, 62, 6);
								MailContent(mail[filteredIndex[curPage * 7 + curLine]], page);
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
							for (int i = nCurPage * 7 + 1; i < min((nCurPage + 1) * 7, filteredIndex.size()); i++)
								UnhoverMailBox(mail[filteredIndex[i]], i - nCurPage * 7);
							HoverMailBox(mail[filteredIndex[nCurPage * 7]], 0);
							curPage = nCurPage;
							curLine = nCurLine;
						}
						else {
							if (curPage != nPage - 1)
								nLine = 7;
							else {
								nLine = filteredIndex.size() % 7;
								if (!nLine && filteredIndex.size()) nLine = 7;
							}
							if (c == DOWN_ARROW)
								nCurLine = (curLine + 1) % nLine;
							else if (c == UP_ARROW)
								nCurLine = (curLine - 1 + nLine) % nLine;
							UnhoverMailBox(mail[filteredIndex[curPage * 7 + curLine]], curLine);
							HoverMailBox(mail[filteredIndex[curPage * 7 + nCurLine]], nCurLine);
						}
						curPage = nCurPage;
						curLine = nCurLine;
					}
				}
				ClearBox(39, HEIGHT, 21, 0);
			}
		}
	}
	mailThread.join();
}