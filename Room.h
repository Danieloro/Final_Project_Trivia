#pragma once
#pragma comment(lib, "ws2_32.lib")

#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<map>
#include<mutex>
#include<queue>

#include "User.h"
#include "Room.h"
#include "RecievedMessage.h"

using namespace std;

class User;
class Room
{
private:
	vector<User*> _users;
	User* _admin;
	int _maxUsers;
	int _questionTime;
	int _questionNo;
	string _name;
	int _id;

public:
	Room(int, User*, string, int, int, int);

	bool joinRoom(User*);
	void leaveRoom(User*);
	int closeRoom(User*);

	vector<User*> getUsers();
	string getUsersListMessage();
	int getQuestionsNo();
	int getID();
	string getName();
	int getMaxUsers() { return _maxUsers; };
	int getQuestionTime() { return _questionTime; };

	string getUsersAsString(vector<User*>, User*);
	void sendMessage(string);
	void sendMessage(User*, string);
};