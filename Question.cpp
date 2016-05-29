#include "Question.h"
#include<iostream>
#include<time.h>

using namespace std;

/*C'tor.*/
Question::Question(int id, string question, string correctAnswer, string answer2, string answer3, string answer4) : _id(id), _question(question)
{
	srand(time(NULL));

	bool foundPlace = false;
	int i;

	_correctAnswerIndex = rand() % 4; //0-3
	_answers = new string[4];

	for (int j = 0; j < 4; j++)
		_answers[j] = "";

	_answers[_correctAnswerIndex] = correctAnswer;

	while (!foundPlace)
	{
		i = rand() % 4;
		if (_answers[i] == "")
		{
			_answers[i] = answer2;
			foundPlace = true;
		}
	}

	foundPlace = false;
	while (!foundPlace)
	{
		i = rand() % 4;
		if (_answers[i] == "")
		{
			_answers[i] = answer3;
			foundPlace = true;
		}
	}

	//Randomizing the last number would be a waste, so ill just find the last empty place.
	if (_answers[0] == "")
		_answers[0] = answer4;
	else if (_answers[1] == "")
		_answers[1] = answer4;
	else if (_answers[2] == "")
		_answers[2] = answer4;
	else
		_answers[3] = answer4;
}

string Question::getQuestion(){ return _question; }

string* Question::getAnswers(){ return _answers; }

int Question::getCorrectAnswerIndex(){ return _correctAnswerIndex; }

int Question::getId(){ return _id; }