#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <conio.h>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>
#include "nlohmann/json.hpp"
#pragma comment(lib, "ws2_32.lib")  // Link với thư viện Winsock
#include <vector>
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
int main() {
    // Khởi tạo Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    // Tạo socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Failed to create socket\n";
        WSACleanup();
        return 1;
    }

    ifstream conf{"config.json"};
    auto j = nlohmann::json::parse(conf);
    const string LOCAL_HOST = j["server"];
    const int PORT_SMTP = j["SMTP"];

    // server
    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(LOCAL_HOST.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_SMTP);
    connect(sock, (sockaddr*)&server, sizeof(server));

    const int buflen = 512;
    char recvbuf[buflen];
    recv(sock, recvbuf, buflen, 0);
    int iResult;
    char sendbuf[buflen] = "";
    bool recvData = 1;

    while (true) {
        cerr << " <- ";
        cin.getline(sendbuf, buflen);
        int n = strlen(sendbuf);
        sendbuf[n++] = '\n';
        sendbuf[n] = '\0';
        iResult = send(sock, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            cerr << "send failed: " << WSAGetLastError() << endl;
            closesocket(sock);
            WSACleanup();
            return 1;
        }
        if (!strcmp(sendbuf, "SEND PICS\n")) {
            string file = "D:\\Code\\MMT\\TH1 - Socket\\treesa.jpg";
            string tmp = Base64::encode(file);
            iResult = send(sock, sendbuf, (int)strlen(sendbuf), 0);
        }
        if (!strcmp(sendbuf, ".\n"))
            recvData = 1;
        if (recvData) {
            memset(recvbuf, 0, buflen);
            iResult = recv(sock, recvbuf, buflen, 0);
            if (iResult == SOCKET_ERROR) {
                cerr << "send failed: " << WSAGetLastError() << endl;
                closesocket(sock);
                WSACleanup();
                return 1;
            }
            cerr << " -> " << recvbuf;
        }
        if (!strcmp(sendbuf, "QUIT\n"))
            break;
        if (!strcmp(sendbuf, "DATA\n"))
            recvData = 0;
    }

    // Đóng kết nối và giải phóng Winsock
    closesocket(sock);
    WSACleanup();
    return 0;
}
