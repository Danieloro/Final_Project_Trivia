#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<map>
#include<mutex>
#include<queue>

#include "RecievedMessage.h"
#include "User.h"
#include "Room.h"
#include "Helper.h"

using namespace std;

/*C'tors*/
RecievedMessage::RecievedMessage(SOCKET s, int messageCode) : _sock(s), _messageCode(messageCode) {}
RecievedMessage::RecievedMessage(SOCKET s, int messageCode, vector<string> values) : _sock(s), _messageCode(messageCode), _values(values) {}

/*Returns the socket*/
SOCKET RecievedMessage::getSock() { return _sock; }

/*Returns the user*/
User* RecievedMessage::getUser() { return _user; }

/*Returns the message code*/
int RecievedMessage::getMessageCode() { return _messageCode; }

/*Returns the values.*/
vector<string>& RecievedMessage::getValues(){ return _values; }

/*Sets a new user.*/
void RecievedMessage::setUser(User* user){ _user = user; }