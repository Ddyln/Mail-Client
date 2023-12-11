#pragma once
#include "Data.h"

int Init(LIST& mail, CONFIG& cnf);
int Recv(SOCKET& sock, char recvbuf[]);
int Send(SOCKET& sock, string& command);
int Find(string s, char c, int i = 0);
int FindNot(string s, char c, int i = 0);
MAIL MsgParser(char s[]);
MAIL RetrMail(SOCKET sock, int i);
void CreateUser(string user);
int FileSize(string path);
bool EnterPath(string& s, int len);
vector <int> FilterMailBySender(const LIST& mailList, const string& sender);
vector <int> FilterMailBySubject(const LIST& mailList, const string& subject);
vector <int> FilterMailByContent(const LIST& mailList, const string& content);
void FilterMail(MAIL& mailm, CONFIG& cnf);