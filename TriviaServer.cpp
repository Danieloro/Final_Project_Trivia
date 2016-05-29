#pragma comment(lib, "ws2_32.lib")

#include "stdafx.h"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "TriviaServer.h"
#include<iostream>
#include <WinSock2.h>
#include <Windows.h>
#include "Helper.h"
#include <thread>
#include <fstream>
#include <condition_variable>
#include "Validator.h"


using namespace std;
const int MAX = 1024;
int TriviaServer::_roomIdSequence = 0;

/*C'tor*/
TriviaServer::TriviaServer()
{

	_uniqueRecievedMessages = unique_lock<mutex>(_mtxRecievedMessages, defer_lock); //Setting the unique lock.
	TriviaServer::_roomIdSequence = 0;
	_db = "C:\\Users\\User\\Desktop\\Daniel\\Magshimim\\C++ Programming 2\\Lesson 8+ Trivia Project\\Trivia\\Debug\\mytempdb.txt";
	
	WSADATA wsa;
	SOCKET s;

	//Starting a winsock library.
	cout << endl << "Initialising Winsock...";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Failed. Error Code : " << WSAGetLastError();
		return;
	}

	cout << " Initialised." << endl;

	//----------------------------------------------------
	//Creates an IPv4 TCP socket!

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		cout << "Could not create socket : " << WSAGetLastError() << endl;
		return;
	}

	cout << "Socket created." << endl;
}

/*D'tor*/
TriviaServer::~TriviaServer()
{
	map<int, Room*>::iterator roomIt;
	map<SOCKET, User*>::iterator userIt;

	for (roomIt = _roomsList.begin(); roomIt != _roomsList.end(); ++roomIt)
		delete roomIt->second;
		

	for (userIt = _connectedUsers.begin(); userIt != _connectedUsers.end(); ++userIt)
		delete userIt->second;

	closesocket(_socket);
}

void TriviaServer::server()
{
	WSADATA wsa;
	SOCKET new_socket;
	struct sockaddr_in server, client;
	char *client_ip;
	int client_port;
	int c;

	//Starting a winsock library.
	cout << endl << "Initialising Winsock...";
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Failed. Error Code : " << WSAGetLastError();
		return;
	}

	cout << " Initialised." << endl;

	//----------------------------------------------------
	//Creates an IPv4 TCP socket!

	if ((_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		cout << "Could not create socket : " << WSAGetLastError() << endl;
		return;
	}

	cout << "Socket created." << endl;

	//----------------------------------------------------
	//Prepating the struct "server"
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(8820);

	//----------------------------------------------------
	//Binding!

	if ((::bind(_socket, (struct sockaddr *)&server, sizeof(server))) == SOCKET_ERROR)
	{
		cout << "Bind failed with error code : " << WSAGetLastError();
		return;
	}

	cout << "Binding finished." << endl;

	//----------------------------------------------------
	//Starting to listen!

	cout << "Listening..." << endl;
	listen(_socket, 3);

	//----------------------------------------------------
	//Accept and incoming connection

	thread t2(&TriviaServer::handleRecievedMessages, this);
	t2.detach();
	while (true)
	{
		cout << "Waiting for incoming connections..." << endl;

		c = sizeof(struct sockaddr_in);
		new_socket = ::accept(_socket, (struct sockaddr *)&client, &c);
		if (new_socket == INVALID_SOCKET)
		{
			cout << "Accept failed with error code : " << WSAGetLastError() << endl;
		}
		else
		{
			client_ip = inet_ntoa(client.sin_addr);
			client_port = ntohs(client.sin_port);

			cout << "Connection accepted: port - " << client_port << " ip - " << client_ip << endl;
			thread t1(&TriviaServer::clientHandler, this, new_socket);
			t1.detach();
			//TriviaServer::clientHandler(new_socket);
		}
	}
}

void TriviaServer::clientHandler(SOCKET s)
{
	int code;

	while ((code = Helper::getMessageTypeCode(s)) != 201)
		TriviaServer::buildRecieveMessage(s, code);
	TriviaServer::buildRecieveMessage(s, code);
}

Room* TriviaServer::getRoomByID(int roomId)
{
	map<int, Room*>::iterator roomIt;

	for (roomIt = _roomsList.begin(); roomIt != _roomsList.end(); ++roomIt)
	{
		if (roomIt->second->getID() == roomId)
			return roomIt->second;
	}

	return nullptr;
}

User* TriviaServer::getUserByName(string username)
{
	map<SOCKET, User*>::iterator userIt;

	for (userIt = _connectedUsers.begin(); userIt != _connectedUsers.end(); ++userIt)
	{
		if (userIt->second->getUsername() == username)
			return userIt->second;
	}

	return nullptr;
}

User* TriviaServer::getUserBySocket(SOCKET client_sock)
{
	map<SOCKET, User*>::iterator userIt;

	for (userIt = _connectedUsers.begin(); userIt != _connectedUsers.end(); ++userIt)
	{
		if (userIt->second->getSocket() == client_sock)
			return userIt->second;
	}

	return nullptr;
}

void TriviaServer::safeDeleteUser(RecievedMessage* msg)
{
	SOCKET clientSock = msg->getSock();

	TriviaServer::handleSignout(msg);

	closesocket(clientSock);
}

User* TriviaServer::handleSignin(RecievedMessage* msg)
{
	vector<string> password_username = msg->getValues();
	User* client_user;

	//CHECK WITH DATABASE IF THE PASSWORD AND USERNAME ARE CORRECT, AND
	//IF NOT SEND AN UNSUCCESSFUL CONNECTION MESSAGE - Helper::sendData(msg->getSock(), "1021");

	if (false) //USE THE DATABASE HERE
	{
		Helper::sendData(msg->getSock(), "1021");
		return nullptr;
	}

	client_user = TriviaServer::getUserByName(password_username[0]); //Finding the user

	if (client_user != nullptr)
	{
		Helper::sendData(msg->getSock(), "1022"); //Sending that the user is already online.
		return nullptr;
	}
	
	client_user = new User(password_username[0], msg->getSock());
	_connectedUsers.insert( pair<SOCKET, User*>(msg->getSock(), client_user) );
	Helper::sendData(msg->getSock(), "1020"); //Sending that the signin was successful
	cout << "Successful signin" << endl;

	return client_user;
}

bool TriviaServer::handleSignup(RecievedMessage* msg)
{
	std::ofstream file;

	if (Validator::isPasswordValid(msg->getValues()[1]))
	{
		if (Validator::isUsernameValid(msg->getValues()[0]))
		{
			if (true) //CHECK IF THE USERNAME ALREADY EXISTS IN THE DATABASE.
			{
				//ADD THE USER TO THE DATABASE.
				file.open(_db, std::ios_base::app);
				file << msg->getValues()[0] << "," << msg->getValues()[1] << endl;
				file.close();

				Helper::sendData(msg->getSock(), "1040");
				cout << "User signed up!" << endl;
				return true;
			}
			else
			{
				Helper::sendData(msg->getSock(), "1042");
				return false;
			}
		}
		else
		{
			Helper::sendData(msg->getSock(), "1043");
			return false;
		}
	}
	else
	{
		Helper::sendData(msg->getSock(), "1041");
		return false;
	}

	Helper::sendData(msg->getSock(), "1044");
	return false;
}

void TriviaServer::handleSignout(RecievedMessage* msg)
{
	if (getUserBySocket(msg->getSock()) != nullptr)
	{
		TriviaServer::handleLeaveRoom(msg);
		TriviaServer::handleCloseRoom(msg);
		TriviaServer::handleLeaveGame(msg);
		_connectedUsers.erase(msg->getSock());
	}
}

void TriviaServer::handleRecievedMessages()
{
	RecievedMessage* currMessage;
	while (true)
	{
		/*Getting the element out of the queue*/
		_uniqueRecievedMessages.lock();

		if (_queRcvMessages.empty())
			_CV.wait(_uniqueRecievedMessages);

		currMessage = _queRcvMessages.front();
		_queRcvMessages.pop();

		_uniqueRecievedMessages.unlock();
		/*------------------------------------*/



		/*Handling the request from the current message.*/
		switch (currMessage->getMessageCode())
		{
		// 200 - The client requests to sign in.
		case 200:
			TriviaServer::handleSignin(currMessage);
			break;

		// 201 - The client requests to disconnect.
		case 201:
			TriviaServer::safeDeleteUser(currMessage);
			break;

		// 203 - The client requests to signup.
		case 203:
			TriviaServer::handleSignup(currMessage);
			break;

		// 205 - The client requests the list of the rooms.
		case 205:
			TriviaServer::handleGetRooms(currMessage);
			break;

		// 207 - The client requests all of the users in his room.
		case 207:
			TriviaServer::handleGetUsersInRoom(currMessage);
			break;

		// 209 - The client requests to join a room.
		case 209:
			TriviaServer::handleJoinRoom(currMessage);
			break;

		// 211 - The client requests to leave his room
		case 211:
			TriviaServer::handleLeaveRoom(currMessage);
			break;

		// 213 - The client requests to create a new room.
		case 213:
			TriviaServer::handleCreateRoom(currMessage);
			break;

		// 215 - The client requests to close the room.
		case 215:
			TriviaServer::handleCloseRoom(currMessage);
			break;

		// 217 - The client requests to start a new game.
		case 217:
			TriviaServer::handleStartGame(currMessage);
			break;

		// 219 - The client sends his answer.
		case 219:
			TriviaServer::handlePlayerAnswer(currMessage);
			break;

		// 222 - The client requests to leave the game.
		case 222:
			TriviaServer::handleLeaveGame(currMessage);
			break;
		
		// 223 - The client requests all of the highscores.
		case 223:
			TriviaServer::handleGetBestScores(currMessage);
			break;

		// 225 The client requests his personal status.
		case 225:
			TriviaServer::handleGetPersonalStatus(currMessage);
			break;

		default:
			TriviaServer::safeDeleteUser(currMessage);
			break;
		}
		/*----------------------------------------------*/
	}
}

void TriviaServer::handleLeaveGame(RecievedMessage* msg)
{
	User* user = msg->getUser();
	bool flag = user->leaveGame();

	if (flag)
	{
		user->clearGame();
	}
}

void TriviaServer::handlePlayerAnswer(RecievedMessage* msg)
{
	Game* temp = msg->getUser()->getGame();
	std::string::size_type sz;
	if (temp != nullptr)
	{
		int ansNum = stoi(msg->getValues()[0], &sz);
		int Time = stoi(msg->getValues()[1], &sz);
		bool flag = temp->handleAnswerFromUser(msg->getUser(), ansNum, Time);
		if (!flag)
		{
			msg->getUser()->clearGame();
		}
	}
}

bool TriviaServer::handleCreateRoom(RecievedMessage* msg)
{
	cout << "Trying to create a room" << endl;
	string temp;
	User* user = msg->getUser();
	if (user != nullptr)
	{
		string name = msg->getValues()[0];
		temp = msg->getValues()[1];
		int maxUsers = stoi(temp);
		temp = msg->getValues()[2];
		int questionNo = stoi(temp);
		temp = msg->getValues()[3];
		int time = stoi(temp);

		TriviaServer::_roomIdSequence++;
		user->createRoom(TriviaServer::_roomIdSequence, name, maxUsers, questionNo, time);
		pair<int, Room*> toAdd = std::make_pair(TriviaServer::_roomIdSequence, user->getRoom());
		_roomsList.insert(toAdd);
		cout << "Successfuly created a room!" << endl;
		return true;
	}
	cout << "Coudlnt create a room, no user!" << endl;
	return false;
}

bool TriviaServer::handleCloseRoom(RecievedMessage* msg)
{
	Room* R = msg->getUser()->getRoom();
	int roomNumber;
	User* user = msg->getUser();
	if (R != nullptr)
	{
		roomNumber = user->closeRoom();
		if (roomNumber != -1)
		{
			this->_roomsList.erase(TriviaServer::_roomIdSequence);
			TriviaServer::_roomIdSequence--;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

void TriviaServer::addRecievedMessage(RecievedMessage* msg)
{
	_uniqueRecievedMessages.lock();

	cout << "Adding request code: " << msg->getMessageCode() << endl;
	this->_queRcvMessages.push(msg);

	_uniqueRecievedMessages.unlock();
	this->_CV.notify_all();
}

void TriviaServer::buildRecieveMessage(SOCKET client_socket, int msgCode)
{
	vector<string> values;
	string valuesToVector;
	string trimmedValues;
	RecievedMessage* message;

	switch (msgCode)
	{
		// 200 - The client requests to signin.
	case 200:
		valuesToVector = Helper::getStringPartFromSocket(client_socket, MAX);
		trimmedValues = TriviaServer::TrimString(valuesToVector);
		break;

		// 203 - The client requests to signup.
	case 203:
		valuesToVector = Helper::getStringPartFromSocket(client_socket, MAX);
		trimmedValues = TriviaServer::TrimString(valuesToVector);
		break;

		// 207 - The client requests all of the users in his room.
	case 207:
		valuesToVector = Helper::getStringPartFromSocket(client_socket, MAX);
		trimmedValues = TriviaServer::TrimString(valuesToVector);
		break;

		// 209 - The client requests to join a room.
	case 209:
		valuesToVector = Helper::getStringPartFromSocket(client_socket, MAX);
		trimmedValues = TriviaServer::TrimString(valuesToVector);
		break;

		// 213 - The client requests to create a new room.
	case 213:
		valuesToVector = Helper::getStringPartFromSocket(client_socket, MAX);
		trimmedValues = TriviaServer::TrimString(valuesToVector);
		break;

		// 219 - The client sends his answer.
	case 219:
		valuesToVector = Helper::getStringPartFromSocket(client_socket, MAX);
		trimmedValues = TriviaServer::TrimString(valuesToVector);
		break;

	default:
		trimmedValues = "";
		break;
	}

	if (trimmedValues == "")
	{
		message = new RecievedMessage(client_socket, msgCode);
		message->setUser(TriviaServer::getUserBySocket(client_socket));
	}
	else
	{
		values = TriviaServer::getValuesFromMessage(trimmedValues, msgCode);
		message = new RecievedMessage(client_socket, msgCode, values);
		message->setUser(TriviaServer::getUserBySocket(client_socket));
	}
	addRecievedMessage(message);
}

/*Before calling this function, call trimstring.*/
vector<string> TriviaServer::getValuesFromMessage(string val, int msgCode)
{
	vector<string> values;
	string temp;
	int size, count = 0;
	
	switch (msgCode)
	{
	// 200 - The client requests to signin.
	case 200:
		count = 0;
		for (int j = 0; j < 2; j++)
		{
			temp = "";

			temp += val[count++];
			temp += val[count++];
			size = stoi(temp);

			temp = "";
			for (int i = 0; i < size; i++, count++)
			{
				temp += val[count];
			}
			values.insert(values.end(), temp);
		}
		break;

	// 203 - The client requests to signup.
	case 203:
		count = 0;
		for (int j = 0; j < 3; j++)
		{
			temp = "";

			temp += val[count++];
			temp += val[count++];
			size = stoi(temp);

			temp = "";
			for (int i = 0; i < size; i++, count++)
			{
				temp += val[count];
			}
			values.insert(values.end(), temp);
		}
		break;

	// 207 - The client requests all of the users in his room.
	case 207:
		values.insert(values.end(), val);
		break;

	// 209 - The client requests to join a room.
	case 209:
		values.insert(values.end(), val);
		break;

	// 213 - The client requests to create a new room.
	case 213:
		count = 0;
		temp = "";

		temp += val[count++];
		temp += val[count++];
		size = stoi(temp);

		temp = "";
		for (int i = 0; i < size; i++, count++)
		{
			temp += val[count];
		}
		values.insert(values.end(), temp);

		temp = val[count++];
		values.insert(values.end(), temp);

		temp = val[count++];
		temp += val[count++];
		values.insert(values.end(), temp);

		temp = val[count++];
		temp += val[count++];
		values.insert(values.end(), temp);

		break;

	// 219 - The client sends his answer.
	case 219:
		count = 0;

		temp = val[0];
		values.insert(values.end(), temp);

		temp = val[1];
		temp += val[2];
		values.insert(values.end(), temp);
		break;
	}

	return values;
}

string TriviaServer::TrimString(string values)
{
	string valuesWithoutJunk = "";
	bool flag = true;

	for (int i = 0; i < MAX && flag; i++)
	{
		if (values[i] >= '0' && values[i] <= '9')
			valuesWithoutJunk += values[i];
		else if (values[i] >= 'A' && values[i] <= 'z')
			valuesWithoutJunk += values[i];
		else if (values[i] >= '!' && values[i] <= '/')
			valuesWithoutJunk += values[i];
		else if (values[i] >= ':' && values[i] <= '@')
			valuesWithoutJunk += values[i];
		else
			flag = true;
	}

	return valuesWithoutJunk;
}

bool TriviaServer::handleJoinRoom(RecievedMessage* msg)
{
	User* user = msg->getUser();
	string questiontime = "", temp;
	vector<User*> allUsers;

	if (user != nullptr)
	{
		temp = msg->getValues()[0];
		Room* R = getRoomByID(stoi(temp));
		cout << "HANDLEJOIN ROOM THE ROOM ID IS " << temp << endl;

		if (R != nullptr)
		{
			user->joinRoom(R);
			questiontime = "1100";
			questiontime += Helper::getPaddedNumber(R->getQuestionsNo(), 2);
			questiontime += Helper::getPaddedNumber(R->getQuestionTime(), 2);
			cout << "handleJoinRoom: sending: " << questiontime << endl;
			user->send(questiontime);

			allUsers = R->getUsers();
			for (int i = 0; i < allUsers.size(); i++)
			{
				cout << "handleJoinRoom: sending: " << R->getUsersListMessage() << endl;
				Helper::sendData(allUsers[i]->getSocket(), R->getUsersListMessage());
			}

			return true;
		}
		else
		{
			user->send("1102");
			return false;
		}
	}
	else
		return false;
}

bool TriviaServer::handleLeaveRoom(RecievedMessage* msg)
{
	User* currUser = msg->getUser();
	Room* currRoom;
	vector<User*> allUsers;

	if (currUser == nullptr)
	{
		cout << "handleLeaveRoom: The user doesnt exit!" << endl;
		return false;
	}

	currRoom = currUser->getRoom();

	if (currRoom == nullptr)
	{
		cout << "handleLeaveRoom: The room doesnt exit!" << endl;
		return false;
	}

	currUser->leaveRoom();

	cout << "HANDLELEAVEROOM - SENDING 1120" << endl;
	currUser->send("1120");

	allUsers = currRoom->getUsers();
	for (int i = 0; i < allUsers.size(); i++)
	{
		cout << "handleJoinRoom: sending: " << currRoom->getUsersListMessage() << endl;
		Helper::sendData(allUsers[i]->getSocket(), currRoom->getUsersListMessage());
	}

	return true;
}

void TriviaServer::handleGetUsersInRoom(RecievedMessage* msg)
{
	User* currUser = msg->getUser();
	string strRoomId = msg->getValues()[0];
	string dataToSend;
	int iRoomId = stoi(strRoomId);
	Room* currRoom;

	currRoom = TriviaServer::getRoomByID(iRoomId);
	if (currRoom == nullptr)
		Helper::sendData(msg->getSock(), "1080");
	else
	{
		dataToSend = currRoom->getUsersListMessage();
		cout << "handleGetUsersInRoom: " << "sending: " << dataToSend << endl;
		Helper::sendData(msg->getSock(), dataToSend);
	}
}

void TriviaServer::handleGetRooms(RecievedMessage* msg)
{
	string Nmsg = "106";
	string temp;
	User* user = msg->getUser();
	map<int, Room*>::iterator it;
	Nmsg += Helper::getPaddedNumber(TriviaServer::_roomIdSequence, 4);
	
	for (it = _roomsList.begin(); it != _roomsList.end(); it++)
	{
		Nmsg += Helper::getPaddedNumber(it->second->getID(), 4);	
		temp = it->second->getName();
		Nmsg += Helper::getPaddedNumber(temp.length(), 2);
		Nmsg += temp;
	}
	if (TriviaServer::_roomIdSequence == 0)
		Nmsg = "1060";
	cout << "handleGetRooms: message sent: " << Nmsg << endl;
	Helper::sendData(msg->getSock(), Nmsg);
}

void TriviaServer::handleGetBestScores(RecievedMessage* msg){}

void TriviaServer::handleGetPersonalStatus(RecievedMessage* msg){}

//////////////////////////////////////////
void TriviaServer::handleStartGame(RecievedMessage* msg)
{
	User* currUser = msg->getUser();
	Room* currRoom = currUser->getRoom();
}
