#include "Data.h"
#include "View.h"
#include "Control.h"
#include "Model.h"
static char recvbuf[buflen] = { 0 };

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
        int n = strlen(s);
        if (n == 2 && s[1] == '.' || n == 0)
            break;
        for (int i = 0; i < n; i++)
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
    vector <string> tmp;
    if (res.line.size() > 1 && res.line[0] == MIME_start) {
        int i = 5;
        while (i < res.line.size() && res.line[i] != res.boundary) tmp.push_back(res.line[i++]);
        tmp.pop_back();
        i++;
        while (res.line.size()) {
            res.att.push_back("");
            while (i < res.line.size() && res.line[i] != res.boundary && res.line[i] != res.boundary + "--") {
                res.att.back() += res.line[i];
                //res.line.erase(res.line.begin() + i);
                i++;
            }
            if (i >= res.line.size() || res.line[i] == res.boundary + "--")
                break;
            i++;
            //res.line.erase(res.line.begin() + i);
        }
        res.line = tmp;
    }
    return res;
}

void CreateUser(string user) {
    system(("mkdir user_data\\" + user + "\\INBOX 2> nul").c_str());
    system(("mkdir user_data\\" + user + "\\PROJECT 2> nul").c_str());
    system(("mkdir user_data\\" + user + "\\IMPORTANT 2> nul").c_str());
    system(("mkdir user_data\\" + user + "\\SPAM 2> nul").c_str());
    system(("mkdir user_data\\" + user + "\\WORK 2> nul").c_str());
    if (FileSize("user_data/" + user + "/mail_id.json") <= 0) {
        ofstream f("user_data/" + user + "/mail_id.json");
        f << "{}";
        f.close();
    }
}
char tmp[buflen] = { 0 };
MAIL RetrMail(SOCKET sock, int i) {
    string command = "RETR ";
    command += to_string(i);
    command += "\n";
    memset(recvbuf, 0, sizeof recvbuf);
    Send(sock, command);
    Recv(sock, recvbuf);
    int sz = 0, pos = 0;
    pos = Find(recvbuf, ' ') + 1;
    while (isdigit(recvbuf[pos])) {
        sz = sz * 10 + (int)recvbuf[pos++] - '0';
    }
    int n = strlen(recvbuf);
    for (i = 0; i < n; i++)
        tmp[i] = recvbuf[i];
    sz -= n;
    while (sz > 0) {
        memset(recvbuf, 0, sizeof recvbuf);
        Recv(sock, recvbuf);
        n = strlen(recvbuf);
        sz -= n;
        for (int j = 0; j < n; j++, i++) {
            tmp[i] = recvbuf[j];
        }
    }
    return MsgParser(tmp);
}

int FileSize(string path) {
    ifstream file(path);
    if (!file) 
        return -1;
    file.seekg(0, std::ios::end);
    streampos fileSize = file.tellg();
    return fileSize;
}

void FilterMail(MAIL& mail, CONFIG& cnf) {
    mail.type = INBOX;
    for (string& text : cnf.spam)
        if (mail.subject.find(text) != string::npos || mail.text.find(text) != string::npos) {
            mail.type = SPAM;
            return;
        }
    for (string& subject : cnf.important)
        if (mail.subject.find(subject) != string::npos) {
            mail.type = IMPORTANT;
            return;
        }
    for (string& content : cnf.work)
        if (mail.text.find(content) != string::npos) {
            mail.type = WORK;
            return;
        }
    for (string& sender : cnf.project) 
        if (mail.from.find(sender) != string::npos) {
            mail.type = PROJECT;
            return;
        }
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
    fstream f{ "user_data/" + cnf.mail + "/mail_id.json" };
    auto data = nlohmann::json::parse(f);
    f.close();
    for (int i = 0; i < n; i++) {
        string ID = mail[i].ID;
        auto it = data.find(ID);
        if (it == data.end()) {
            mail[i] = (RetrMail(sock, i + 1));
            //exit(0);
            mail[i].ID = ID;
            FilterMail(mail[i], cnf);
            string path = "user_data/" + cnf.mail + "/";
            if (mail[i].type == INBOX)
                path += "INBOX/";
            else if (mail[i].type == IMPORTANT)
                path += "IMPORTANT/";
            else if (mail[i].type == PROJECT)
                path += "PROJECT/";
            else if (mail[i].type == WORK)
                path += "WORK/";
            else if (mail[i].type == SPAM)
                path += "SPAM/";
            path += ID + ".json";
            ofstream file(path);
            file << "{}";
            file.close();

            fstream f{ path };
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

            file.open(path);
            file << setw(4) << data;
            file.close();
        }
        else {
            int t = INBOX;
            {
                fstream f{ "user_data/" + cnf.mail + "/mail_id.json" };
                auto data = nlohmann::json::parse(f); 
                f.close();
                t = data[ID];
            }
            string path = "user_data/" + cnf.mail + "/";
            if (t == INBOX)
                path += "INBOX/";
            else if (t == IMPORTANT)
                path += "IMPORTANT/";
            else if (t == PROJECT)
                path += "PROJECT/";
            else if (t == WORK)
                path += "WORK/";
            else if (t == SPAM)
                path += "SPAM/";
            path += ID + ".json";
            fstream f{ path };
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
        data[mail[i].ID] = mail[i].type;
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

bool EnterText(string& s, int len) {
    unsigned char c;
    HideCursor(0);
    while (true) {
        c = _getch();
        if (c == ENTER) {
            break;
        }
        else if (c == ESC) {
            int n = min(s.size(), len);
            GotoXY(WhereX() - n, WhereY());
            for (int i = 0; i < n; i++)
                cout << ' ';
            s = "";
            HideCursor(1);
            return 0;
        }
        else if (c == BACK_SPACE) {
            if (s.size()) {
                s.pop_back();
                if (s.size() + 1 < len) {
                    GotoXY(WhereX() - 1, WhereY());
                    cout << ' ';
                    GotoXY(WhereX() - 1, WhereY());
                }
                else {
                    GotoXY(WhereX() - len + 1, WhereY());
                    int n = s.size();
                    cout << s.substr(n - len + 1);
                }
            }
        }
        else if (c == 224) {
            c = _getch();
        }
        else if (c != TAB) {
            s += c;
            if (s.size() < len)
                cout << c;
            else {
                GotoXY(WhereX() - len + 1, WhereY());
                int n = s.size();
                cout << s.substr(n - len + 1);
            }
        }
    }
    HideCursor(1);
    return 1;
}
