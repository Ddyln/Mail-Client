#include "Data.h"
#include "View.h"
#include "Control.h"
#include "Model.h"

int Recv(SOCKET& sock, char recvbuf[]) {
    memset(recvbuf, 0, sizeof recvbuf);
    int result = recv(sock, recvbuf, buflen, 0);
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
        memset(s, 0, buflen);
        inp.getline(s, buflen, 13);
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
        memset(s, 0, buflen);
        inp.getline(s, buflen, 13);
        if (strlen(s) == 2 && s[1] == '.')
            break;
        for (int i = 0; i < strlen(s); i++)
            res.text += s[i];
    }

    res.text.erase(0, 1);
    res.line.push_back("");
    for (int i = 0; i < res.text.size(); i++) {
        if (res.line.back().size() == 57 || res.text[i] == '\n') {
            res.line.push_back("");
            if (res.text[i] == '\n')
                continue;
        }
        res.line.back().push_back(res.text[i]);
    }
    return res;
}

void CreateUser(string user) {
    system(("mkdir " + user + " 2> nul").c_str());
    system(("move " + user + " user_data 2> nul > nul").c_str());
    system(("rmdir " + user + " 2> nul").c_str());
}

MAIL RetrMail(SOCKET sock, int i) {
    string command = "RETR ";
    command += to_string(i);
    command += "\n";
    char recvbuf[buflen] = { 0 };
    Send(sock, command);
    Recv(sock, recvbuf);
    return MsgParser(recvbuf);
}

int FileSize(string path) {
    ifstream file(path);
    if (!file) 
        return -1;
    file.seekg(0, std::ios::end);
    streampos fileSize = file.tellg();
    return fileSize;
}

int Init(LIST& mail, CONFIG& cnf) {
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
    string conf_server = cnf.server;
    int conf_POP3 = cnf.POP3;
    sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(conf_server.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(conf_POP3);
    connect(sock, (sockaddr*)&server, sizeof(server));

    char recvbuf[buflen] = { 0 };
    Recv(sock, recvbuf);

    string command = "USER ";
    command += cnf.mail;
    command += "\n";
    Send(sock, command);
    Recv(sock, recvbuf);

    command = "PASS ";
    command += cnf.password;
    command += "\n";
    Send(sock, command);
    Recv(sock, recvbuf);

    command = "STAT\n";
    Send(sock, command);
    Recv(sock, recvbuf);

    memset(recvbuf, 0, buflen);
    Send(sock, command);
    Recv(sock, recvbuf);

    int i = 4, n = 0;
    while (isdigit(recvbuf[i])) {
        n = n * 10 + (recvbuf[i++] - '0');
    }

    for (int i = 1; i <= n; i++)   
        mail.push_back(RetrMail(sock, i));

    memset(recvbuf, 0, buflen);
    command = "UIDL\n";
    Send(sock, command);
    Recv(sock, recvbuf);
    int pos = 0;
    for (int i = 0; i < n; i++) {
        pos = Find(recvbuf, ' ', pos);
        int tmp = pos;
        mail[i].ID = (string(recvbuf)).substr(pos + 1, (tmp = Find(recvbuf, '.', pos)) - pos - 1);
        pos = tmp;
    }
    command = "QUIT\n";
    Send(sock, command);
    Recv(sock, recvbuf);
    closesocket(sock);
    WSACleanup();

    CreateUser(cnf.mail);
    if (FileSize("user_data/" + cnf.mail + "/mail_id.json") <= 0) {
        ofstream f("user_data/" + cnf.mail + "/mail_id.json");
        f << "{}";
        f.close();
    }
    ifstream f{ "user_data/" + cnf.mail + "/mail_id.json" };
    auto data = nlohmann::json::parse(f);
    f.close();

    for (int i = 0; i < n; i++) {
        auto it = data.find(mail[i].ID);
        if (it == data.end()) {
            data[mail[i].ID] = 1;
            ofstream file("user_data/" + cnf.mail + "/" + mail[i].ID + ".json");
            file << "{}";
            file.close();

            ifstream f{"user_data/" + cnf.mail + "/" + mail[i].ID + ".json"};
            auto data = nlohmann::json::parse(f);
            f.close();

            data["subject"] = mail[i].subject;
            data["from"] = mail[i].from;
            data["to"] = mail[i].to;
            data["cc"] = mail[i].cc;
            data["ID"] = mail[i].ID;
            data["line"] = mail[i].line;
            data["text"] = mail[i].text;
            data["type"] = mail[i].type;

            file.open("user_data/" + cnf.mail + "/" + mail[i].ID + ".json");
            file << setw(4) << data;
            file.close();
        }
    }
    ofstream file("user_data/" + cnf.mail + "/mail_id.json");
    file << setw(4) << data;
    file.close();
    return 0;
}