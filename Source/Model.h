#pragma once
#include "Data.h"

int Init(LIST& mail);
int Recv(SOCKET& sock, char recvbuf[]);
int Send(SOCKET& sock, string& command);
int Find(string s, char c, int i = 0);
int FindNot(string s, char c, int i = 0);
MAIL MsgParser(char s[]);
MAIL RetrMail(SOCKET sock, int i);