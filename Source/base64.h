#pragma once
#include <fstream>
#include <vector>
#include "Control.h"
using namespace std;

const string f = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

struct Base64 {
	static string encode(string file) {
		string res = "";
		ifstream inp(file, ios::binary);
		char buf[2] = { 0 };
		int val = 0, valb = -6;
		while (true) {
			memset(buf, 0, sizeof buf);
			inp.read(buf, 1);
			if (inp.eof())
				break;
			val = (val << 8) + *(short*)buf;
			valb += 8;
			while (valb >= 0) {
				res.push_back(f[(val >> valb) & 0x3F]);
				valb -= 6;
			}
		}
		if (valb > -6) res.push_back(f[((val << 8) >> (valb + 8)) & 0x3F]);
		while (res.size() % 4) res.push_back('=');
		inp.close();
		return res;
	}

	static void decode(string in, string dest) {
		ofstream out(dest, ios::binary);
		vector<int> T(256, -1);
		for (int i = 0; i < 64; i++) T[f[i]] = i;
		int val = 0, valb = -8;
		for (char c : in) {
			if (T[c] == -1) break;
			val = (val << 6) + T[c];
			valb += 6;
			if (valb >= 0) {
				char tmp = ((val >> valb) & 0xFF);
				char s[2] = { 0 };
				s[0] = tmp;
				out.write(s, 1);
				valb -= 8;
			}
		}
		out.close();
	}
};