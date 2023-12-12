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
bool EnterText(string& s, int len);
void FilterMail(MAIL& mailm, CONFIG& cnf);
