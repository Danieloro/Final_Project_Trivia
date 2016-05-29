/********************************************

IMPORTANT:
I didnt debug this class, so there may be problems.
If you use this class and there are no errors/problems, please delete this message.
If you encounter any error during your use of this class, please contact me.
Good luck!

********************************************/

#include "User.h"
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<map>
#include<mutex>
#include<queue>

#include "Helper.h"
#include "Game.h"
#include "Room.h"

using namespace std;

/*C'tor of this class.*/
User::User(string username, SOCKET sock) : _user(username), _sock(sock)
{
	_currGame = nullptr;
	_currRoom = nullptr;
}

/*Sends "message"(string) to the user.*/
void User::send(string message)
{
	try
	{
		Helper::sendData(_sock, message);
	}
	catch (string err)
	{
		cout << "ERROR: " << err << endl;
	}
}

/*Returns the name of the username.*/
string User::getUsername() { return _user; }

/*Returns the socket.*/
SOCKET User::getSocket(){ return _sock; }

/*Returns the room.*/
Room* User::getRoom(){ return _currRoom; }

/*Returns the game.*/
Game* User::getGame(){ return _currGame; }

/*Sets a new game, and */
void User::setGame(Game* gm)
{
	_currGame = gm;
	_currRoom = nullptr;
}

/*The game object will be nullptr.*/
void User::clearGame()
{
	_currGame = nullptr;
}

/*Creates a new room, Returns wether the room was created successfuly or not.*/
bool User::createRoom(int roomID, string roomName, int maxUsers, int questionNo, int questionTime)
{
	if (_currRoom == nullptr)
	{
		_currRoom = new Room(roomID, this, roomName, maxUsers, questionNo, questionTime);
		Helper::sendData(_sock, "1140"); //The room was created successfuly.
		return true;
	}

	Helper::sendData(_sock, "1141"); //The room wasnt created due to the user already being in a room.
	return false;
}

/*Joins a room!*/
bool User::joinRoom(Room* newRoom)
{
	if (_currRoom != nullptr)
		return false;

	_currRoom = newRoom;
	_currRoom->joinRoom(this);
	return true;

}

/*Leaves the room.*/
void User::leaveRoom()
{
	if (_currRoom != nullptr)
	{
		_currRoom->leaveRoom(this); //Notifing the room that im leaving.
		_currRoom = nullptr; //Actually leaving
	}

}

/*Closing the room.*/
int User::closeRoom()
{
	int roomNumber;

	if (_currRoom == nullptr)
		return -1;

	roomNumber = _currRoom->getID();

	_currRoom->closeRoom(this);
	delete _currRoom;
	_currRoom = nullptr;

	return roomNumber;
	

}

/*Leave the game.*/
bool User::leaveGame()
{
	if (_currGame == nullptr)
		return false;
	
	return _currGame->leaveGame(this);
}