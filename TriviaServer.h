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
#include<condition_variable>

#include "User.h"
#include "Room.h"
#include "RecievedMessage.h"

using namespace std;

class TriviaServer
{
private:
	SOCKET _socket;
	map<SOCKET, User*> _connectedUsers;
	string _db; //DataBase, we should change it later.
				//Meanwhile, it will be the name of the file.
	map<int, Room*> _roomsList;
	mutex _mtxRecievedMessages;
	unique_lock<mutex> _uniqueRecievedMessages;
	queue<RecievedMessage*> _queRcvMessages;
	static int _roomIdSequence;
	condition_variable _CV;

public:
	//C'tor + D'tor
	TriviaServer(); //DONE
	~TriviaServer(); //DONE

	void server(); //DONE

	void clientHandler(SOCKET); //DONE
	void safeDeleteUser(RecievedMessage*); //DONE

	User* handleSignin(RecievedMessage*); //DONE
	bool handleSignup(RecievedMessage*); //DONE
	void handleSignout(RecievedMessage*); //DONE

	void handleLeaveGame(RecievedMessage*); //DENNIS DID IT.
	void handleStartGame(RecievedMessage*); //////////////////////////////////////////////
	void handlePlayerAnswer(RecievedMessage*); //DENNIS DID IT.

	bool handleCreateRoom(RecievedMessage*); //DENNIS DID IT.
	bool handleCloseRoom(RecievedMessage*); //DENNIS DID IT
	bool handleJoinRoom(RecievedMessage*); //DENNIS DID IT
	bool handleLeaveRoom(RecievedMessage*); //DONE
	void handleGetUsersInRoom(RecievedMessage*); //DONE
	void handleGetRooms(RecievedMessage*); //DENNIS DID IT

	void handleGetBestScores(RecievedMessage*); ////////////////////////////////////////////
	void handleGetPersonalStatus(RecievedMessage*); /////////////////////////////////////////

	void handleRecievedMessages(); //DONE
	void addRecievedMessage(RecievedMessage*); //DENNIS DID IT.
	void buildRecieveMessage(SOCKET, int); //DENNIS DID IT
	vector<string> getValuesFromMessage(string, int);
	string TrimString(string);

	User* getUserByName(string); //DONE
	User* getUserBySocket(SOCKET); //DONE
	Room* getRoomByID(int);//DONE
};