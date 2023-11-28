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
#pragma comment(lib, "ws2_32.lib")  // Link với thư viện Winsock
#define LOCAL_HOST "127.0.0.1"
#define PORT_POP3 3335
using namespace std;

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

    // server
    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT_POP3);
    connect(sock, (sockaddr*)&server, sizeof(server));

    const int buflen = 512;
    char recvbuf[buflen] = "";
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
