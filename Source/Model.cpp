#include "Data.h"
#include "View.h"
#include "Control.h"
#include "Model.h"

int Recv(SOCKET& sock, char recvbuf[]) {
    memset(recvbuf, 0, sizeof recvbuf);
    int result = recv(sock, recvbuf, 512, 0);
    if (result == SOCKET_ERROR) {
        cerr << "send failed: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    return 0;
}

int Send(SOCKET& sock, string& command) {
    int result = send(sock, command.c_str(), command.size(), 0);
    if (result == SOCKET_ERROR) {
        cerr << "send failed: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    return 0;
}

int Find(string s, char c, int i) {
    while (i < s.size())
        if (s[i] == c)
            return i;
        else
            i++;
    return -1;
}

int FindNot(string s, char c, int i) {
    while (i < s.size())
        if (s[i] != c)
            return i;
        else
            i++;
    return -1;
}

MAIL MsgParser(char s[]) {
    stringstream inp(s);
    MAIL res;

    while (true) {
        memset(s, 0, 512);
        inp.getline(s, 512, 13);
        if (strlen(s) == 1 && s[0] == '\n')
            break;
        int pos = Find(s, ':');
        if (pos < 0) continue;
        string type = (string(s)).substr(1, pos - 1);
        if (type == "Subject")
            res.subject = (string(s)).substr(FindNot(s, ' ', pos + 1), strlen(s));
        else if (type == "From")
            res.from = (string(s)).substr(FindNot(s, ' ', pos + 1), strlen(s));
        else if (type == "To")
            res.to = (string(s)).substr(FindNot(s, ' ', pos + 1), strlen(s));
        else if (type == "Cc")
            res.cc = (string(s)).substr(FindNot(s, ' ', pos + 1), strlen(s));
    }

    while (true) {
        memset(s, 0, 512);
        inp.getline(s, 512, 13);
        if (strlen(s) == 2 && s[1] == '.')
            break;
        for (int i = 0; i < strlen(s); i++)
            res.text += s[i];
    }
    res.text.erase(0, 1);
    return res;
}

MAIL RetrMail(SOCKET sock, int i) {
    string command = "RETR ";
    command += char(i + '0');
    command += "\n";
    char recvbuf[512] = { 0 };
    Send(sock, command);
    Recv(sock, recvbuf);
    return MsgParser(recvbuf);
}

int Init(LIST& mail) {
	ifstream f{ "config.json" };
	auto conf = nlohmann::json::parse(f);

	WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Failed to create socket\n";
        WSACleanup();
        return 1;
    }

    string conf_server = conf["server"];
    int conf_POP3 = conf["POP3"];
    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(conf_server.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(conf_POP3);
    connect(sock, (sockaddr*)&server, sizeof(server));

    char recvbuf[512] = { 0 };
    Recv(sock, recvbuf);

    string command = "USER ";
    command += conf["mail"];
    command += "\n";
    Send(sock, command);
    Recv(sock, recvbuf);

    command = "PASS ";
    command += conf["password"];
    command += "\n";
    Send(sock, command);
    Recv(sock, recvbuf);

    command = "STAT\n";
    Send(sock, command);
    Recv(sock, recvbuf);

    memset(recvbuf, 0, 512);
    Send(sock, command);
    Recv(sock, recvbuf);

    int i = 4, n = 0;
    while (isdigit(recvbuf[i])) {
        n = n * 10 + (recvbuf[i++] - '0');
    }

    MAIL tempMail;
    for (int i = 1; i <= n; i++) 
        mail.push_back(RetrMail(sock, i));

    command = "QUIT\n";
    Send(sock, command);
    Recv(sock, recvbuf);

    closesocket(sock);
    WSACleanup();
    return 0;
}