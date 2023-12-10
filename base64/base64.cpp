#include "base64.h"

static string Base64::Encode(string file) {
	string res = "";
	ifstream inp(file, ios::binary);
	char cur[7] = { 0 };
	while (inp.good()) {
		memset(cur, 0, sizeof cur);
		inp.read(cur, 6);
		int id = 0;
		for (int i = 0; i < strlen(cur); i++)
			id += (1 << i);
		res += f[id];
	}
	return res;
	inp.close();
}