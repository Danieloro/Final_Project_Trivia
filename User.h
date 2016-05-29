#pragma once
#pragma comment(lib, "ws2_32.lib")

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<map>
#include<mutex>
#include<queue>

#include "Game.h"
#include "Room.h"

using namespace std;

class Game;
class Room;
class User
{
private:
	string _user;
	Room* _currRoom;
	Game* _currGame;
	SOCKET _sock;

public:
	User(string, SOCKET); //DONE
	
	void send(string); //DONE
	string getUsername(); //DONE
	SOCKET getSocket(); //DONE
	Room* getRoom(); //DONE
	Game* getGame(); //DONE
	void setGame(Game*); //DONE
	void clearGame();  //DONE
	bool createRoom(int, string, int, int, int); //DONE
	bool joinRoom(Room*); //DONE
	void leaveRoom(); //DONE
	int closeRoom(); //DONE
	bool leaveGame(); //DONE
};