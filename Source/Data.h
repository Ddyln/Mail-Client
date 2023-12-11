#pragma once
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <iostream>
#include <cctype>
#include <Windows.h>
#include <conio.h>
#include <iomanip>
#include <fstream>
#include <vector>
#include <math.h>
#include <string>
#include <mmsystem.h>
#include <windows.h>
#include <queue>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include "nlohmann/json.hpp"
#include "base64.h"
using namespace std;
#pragma comment(lib, "ws2_32.lib")

// Console size
#define WIDTH 121
#define HEIGHT 31
#define buflen 4194304
#define MaxChar 57

// Color code
#define BLACK (15 << 4)
#define BLUE ((15 << 4) | 1)
#define GREEN ((15 << 4) | 2)
#define CYAN ((15 << 4) | 3)
#define RED ((15 << 4) | 4)
#define MAGENTA ((15 << 4) | 5)
#define YELLOW ((15 << 4) | 6)
#define WHITE ((15 << 4) | 7)
#define GRAY ((15 << 4) | 8)
#define LIGHT_BLUE ((15 << 4) | 9)
#define LIGHT_GREEN ((15 << 4) | 10)
#define LIGHT_CYAN ((15 << 4) | 11)
#define LIGHT_RED ((15 << 4) | 12)
#define LIGHT_MAGENTA ((15 << 4) | 13)
#define LIGHT_YELLOW ((15 << 4) | 14)
#define BRIGHT_WHITE ((15 << 4) | 15)
#define BACKGROUND_YELLOW (14 << 4)
#define BACKGROUND_RED (12 << 4)

// ASCII code
#define ENTER 13
#define ESC 27
#define BACK_SPACE 8
#define H_LINE (char)196
#define V_LINE (char)179
#define CROSS (char)197
#define TOP_LEFT (char)218
#define TOP_RIGHT (char)191
#define BOTTOM_LEFT (char)192
#define BOTTOM_RIGHT (char)217
#define TOP_CROSS (char)194
#define BOTTOM_CROSS (char)193
#define	LEFT_CROSS (char)195
#define RIGHT_CROSS (char)180
#define L_TRIANGLE ((char)16)
#define R_TRIANGLE ((char)17)
// for drawing boxes (menu, etc.)
#define BOX_TOP_LEFT (char)201
#define BOX_TOP_RIGHT (char)187
#define BOX_BOTTOM_LEFT (char)200
#define BOX_BOTTOM_RIGHT (char)188
#define BOX_RIGHT (char)185
#define BOX_LEFT (char)204
#define BOX_V_LINE (char)186
#define BOX_H_LINE (char)205
#define BOX_X  WIDTH / 2 - 39
#define BOX_Y HEIGHT / 2 - 13
#define BOX_W 40
#define BOX_H 20
#define SPACE (char)32
#define LEFT_ARROW 75
#define UP_ARROW 72
#define RIGHT_ARROW 77
#define DOWN_ARROW 80

const string MIME_start = "This is a multi-part message in MIME format.";
enum FILTER{
	INBOX,
	PROJECT,
	IMPORTANT,
	SPAM,
	WORK
};

struct CONFIG {
	string username = "", mail = "", password = "", server = "";
	int SMTP = 0, POP3 = 0, autoload = 0;
	vector <string> filteredBySender, important, work, project, spam;
};

struct MAIL {
	string subject = "", from = "", to = "", cc = "", ID = "";
	vector <string> line;
	vector <string> att;
	string text = "";
	string boundary = "";
	FILTER type = INBOX;
	bool read = false;

	void markAsRead(CONFIG& cnf) {
		read = true;
		string path = "user_data/" + cnf.mail + "/";
		if (type == INBOX)
			path += "INBOX/";
		else if (type == IMPORTANT)
			path += "IMPORTANT/";
		else if (type == PROJECT)
			path += "PROJECT/";
		else if (type == WORK)
			path += "WORK/";
		else if (type == SPAM)
			path += "SPAM/";
		path += ID + ".json";
		fstream f{ path };
		auto data = nlohmann::json::parse(f);
		f.close();
		ofstream out{ path };
		data["read"] = read;
		out << setw(4) << data;
		out.close();
	}
};

typedef vector <MAIL> LIST;