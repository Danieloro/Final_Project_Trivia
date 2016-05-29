#pragma once
#pragma comment(lib, "ws2_32.lib")

#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<map>
#include<mutex>
#include<queue>

#include "Helper.h"
#include "User.h"

using namespace std;

class User;
class RecievedMessage
{
private:
	SOCKET _sock;
	int _messageCode;
	vector<string> _values;
	User* _user;

public:
	RecievedMessage(SOCKET, int);
	RecievedMessage(SOCKET, int, vector<string>);

	SOCKET getSock();
	User* getUser();
	int getMessageCode();
	vector<string>& getValues();

	void setUser(User*);
};