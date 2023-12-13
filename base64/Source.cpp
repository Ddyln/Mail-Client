#include <iostream>
#include <string>
#include <stack>
#include "base64.h"

using namespace std;
int FileSize(string path) {
    ifstream file(path);
    if (!file)
        return -1;
    file.seekg(0, std::ios::end);
    streampos fileSize = file.tellg();
    return fileSize;
}

int main() {
    string ext = ".pdf";
	string tmp = Base64::encode("test" + ext);
    cerr << tmp.size() << endl;
    Base64::decode(tmp, "decoded" + ext);

	return 0;
}