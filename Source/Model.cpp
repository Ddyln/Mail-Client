#include "Data.h"
#include "View.h"
#include "Control.h"
#include "Model.h"
char recvbuf[buflen] = { 0 };

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
        else if (type == "Content-Type") {
            if ((string(s)).substr(pos + 1, Find(s, ';', pos) - (pos + 1)) == " multipart/mixed") {
                pos = Find(s, 'b', pos);
                pos = Find(s, '"', pos);
                res.boundary = "--" + (string(s)).substr(pos + 1, Find(s, '"', pos + 1) - (pos + 1));
            }
        }
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
    if (res.line.size() > 1 && res.line[0] == MIME_start) {
        int i = 0;
        for (int t = 0; t < 5; t++)
            res.line.erase(res.line.begin() + i);
        while (i < res.line.size() && res.line[i] != res.boundary) i++;
        i--;
        for (int t = 0; t < 2; t++)
            res.line.erase(res.line.begin() + i);

        while (res.line.size()) {
            res.att.push_back("");
            while (i < res.line.size() && res.line[i] != res.boundary) {
                for (int j = 0; j < res.line[i].size(); j++)
                    res.att.back() += res.line[i][j];
                res.line.erase(res.line.begin() + i);
            }
            while (i < res.line.size() && res.line[i] != res.boundary) i++;
            if (i >= res.line.size())
                break;
            res.line.erase(res.line.begin() + i);
        }
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
    memset(recvbuf, 0, sizeof recvbuf);
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

    memset(recvbuf, 0, sizeof recvbuf);
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
    mail.resize(n);

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

    CreateUser(cnf.mail);
    if (FileSize("user_data/" + cnf.mail + "/mail_id.json") <= 0) {
        ofstream f("user_data/" + cnf.mail + "/mail_id.json");
        f << "{}";
        f.close();
    }
    fstream f{ "user_data/" + cnf.mail + "/mail_id.json" };
    auto data = nlohmann::json::parse(f);
    f.close();

    for (int i = 0; i < n; i++) {
        string ID = mail[i].ID;
        auto it = data.find(ID);
        if (it == data.end()) {
            mail[i] = (RetrMail(sock, i + 1));
            mail[i].ID = ID;
            ofstream file("user_data/" + cnf.mail + "/" + mail[i].ID + ".json");
            file << "{}";
            file.close();

            fstream f{ "user_data/" + cnf.mail + "/" + mail[i].ID + ".json" };
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
            data["boundary"] = mail[i].boundary;
            data["att"] = mail[i].att;
            data["read"] = mail[i].read;

            file.open("user_data/" + cnf.mail + "/" + mail[i].ID + ".json");
            file << setw(4) << data;
            file.close();
        }
        else {
            fstream f{ "user_data/" + cnf.mail + "/" + mail[i].ID + ".json" };
            auto data = nlohmann::json::parse(f);
            f.close();
            mail[i].from = data["from"];
            mail[i].to = data["to"];
            mail[i].subject = data["subject"];
            mail[i].cc = data["cc"];
            mail[i].ID = data["ID"];
            mail[i].line = data["line"];
            mail[i].type = data["type"];
            mail[i].boundary = data["boundary"];
            mail[i].att = data["att"];
            mail[i].read = data["read"];
        }
        data[mail[i].ID] = 1;
    }

    command = "QUIT\n";
    Send(sock, command);
    Recv(sock, recvbuf);
    closesocket(sock);
    WSACleanup();

    ofstream file("user_data/" + cnf.mail + "/mail_id.json");
    file << setw(4) << data;
    file.close();

    return 0;
}

bool EnterPath(string& s, int len) {
    char c;
    HideCursor(0);
    while (true) {
        c = _getch();
        if (c == ENTER) {
            break;
        }
        else if (c == ESC) {
            s = "";
            HideCursor(1);
            return 0;
        }
        else if (c == BACK_SPACE) {
            if (s.size()) {
                int x = WhereX(), y = WhereY();
                GotoXY(x - 1, y);
                cout << ' ';
                GotoXY(x - 1, y);
                s.pop_back();
            }
        }
        else if (s.size() < len && c < 128) {
            cout << c;
            s += c;
        }
    }
    HideCursor(1);
    return 1;
}

//lọc = người gửi
LIST FilterMailBySender(const LIST& mailList, const string& sender) {
    LIST filteredList;
    copy_if(mailList.begin(), mailList.end(), back_inserter(filteredList),
        [sender](const MAIL& mail) { return mail.from == sender; });
    return filteredList;
}

//lọc = subject
LIST FilterMailBySubject(const LIST& mailList, const string& subject) {
    LIST filteredList;
    copy_if(mailList.begin(), mailList.end(), back_inserter(filteredList),
        [subject](const MAIL& mail) { return mail.subject == subject; });
    return filteredList;
}

//lọc = nội dung
LIST FilterMailByContent(const LIST& mailList, const string& content) {
    LIST filteredList;
    copy_if(mailList.begin(), mailList.end(), back_inserter(filteredList),
        [content](const MAIL& mail) { return mail.text.find(content) != string::npos; });
    return filteredList;
}