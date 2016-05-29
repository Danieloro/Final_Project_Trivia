#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<map>
#include<mutex>
#include<vector>
#include<queue>
#include <string>
#include <fstream>

#include "Game.h"
#include "User.h"
#include "Room.h"
#include "RecievedMessage.h"
#include "Helper.h"

using namespace std;

Game::Game()
{

}

Game::~Game()
{

}

void Game::sendQuestionToAllUsers()
{
	this->_currentTurnAnswers = 0;
}

void Game::handleFinishGame()
{

}

void Game::sendFirstQuestion()
{

}

bool Game::handleNextTurn()
{
	return true;
}

bool Game::handleAnswerFromUser(User* user, int answerNo, int time)
{
	return true;
}

bool Game::leaveGame(User* currUser)
{
	return true;
}