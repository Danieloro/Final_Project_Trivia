#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<map>
#include<mutex>
#include<vector>
#include<queue>
#include <string>

#include "User.h"
#include "Room.h"
#include "RecievedMessage.h"
#include "Helper.h"

using namespace std;

Room::Room(int id, User* admin, string name, int maxUsers, int questionsNo, int questionTime) :
_id(id), _admin(admin), _name(name), _maxUsers(maxUsers), _questionNo(questionsNo), _questionTime(questionTime)
{
	_users.insert(_users.end(), admin);
}

bool Room::joinRoom(User* user)
{
	string updateAllUsers;
	if (_users.size() < _maxUsers) //if there is place inside the room
	{
		_users.insert(_users.end(), user);
		//user->send("succsess");
		//updateAllUsers = getUsersListMessage();
		//sendMessage(updateAllUsers);
		return true;
	}
	else
	{
		//user->send("failure");
		return false;
	}
}

void Room::leaveRoom(User* user)
{
	int i = 0;
	for (int i = 0; i < _users.size(); i++)
	{
		if (_users[i]->getUsername() == user->getUsername())
		{
			vector<User*>::iterator it = _users.begin() + i;
			_users.erase(it);
		}
	}
}

int Room::closeRoom(User* user)
{
	if (user->getUsername() == _admin->getUsername()) //if the user is the room's admin
	{
		for (int i = 0; i < _users.size(); i++) //goes throughout all active users
		{
			_users[i]->send("116");
		}

		for (int i = 0; i < _users.size(); i++) //goes throughout all active users
		{
			if (_users[i]->getUsername() != _admin->getUsername()) //exept the admin
				_users[i]->clearGame();
		}
		return _id;
	}
	else
	{
		return -1;
	}
}

vector<User*> Room::getUsers()
{
	return _users;
}

string Room::getUsersListMessage()
{
	User* temp;
	string M;
	string sizeName = "";
	int size;

	M = "108";
	if (_users.size() == 0)
	{
		return M + "0";
	}
	else
	{
		M += to_string(_users.size());
		for (int i = 0; i < _users.size(); i++) //goes throughout all active users
		{
			temp = _users[i];
			size = temp->getUsername().length();
			sizeName = Helper::getPaddedNumber(size, 2);
			M += sizeName + temp->getUsername();
		}
		return M;
	}
}

int Room::getQuestionsNo()
{
	return _questionNo;
}

int Room::getID()
{
	return _id;
}

string Room::getName()
{
	return _name;
}

string Room::getUsersAsString(vector<User*> users, User* user)
{
	string names = "";
	for (int i = 0; i < users.size(); i++)
	{
		if (users[i]->getUsername() != user->getUsername()) //goes throughout all active users
		{
			if ((i + 1) == users.size())
				names += users[i]->getUsername();
			else
				names += users[i]->getUsername() + " ";
		}
	}
	return names;
}

void Room::sendMessage(string M)
{
	this->sendMessage(NULL, M);
}

void Room::sendMessage(User* user, string M)
{
	for (int i = 0; i < _users.size(); i++) //goes throughout all active users
	{
		if (_users[i]->getUsername() != user->getUsername())
			_users[i]->send(M);
	}
}