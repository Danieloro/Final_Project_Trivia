#pragma once
#pragma comment(lib, "ws2_32.lib")

#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<map>
#include<mutex>
#include<queue>
#include <fstream>

#include "Game.h"
#include "User.h"
#include "Room.h"
#include "RecievedMessage.h"

using namespace std;
class Game
{
private:
	std::map<std::string, int> _results;
	int _currentTurnAnswers;

public:
	Game(/*const vector<User*>& players, int questionsNo*/ /*DataBase& db*/);
	~Game();
	void sendQuestionToAllUsers();
	void handleFinishGame();
	void sendFirstQuestion();
	bool handleNextTurn();
	bool handleAnswerFromUser(User* user, int answerNo, int time);
	bool leaveGame(User* currUser);
	std::map<std::string, int> getResults() { return _results; }
	int getCurrTurnAns() { return _currentTurnAnswers; }
};