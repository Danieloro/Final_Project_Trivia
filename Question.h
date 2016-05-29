#pragma once

#include<iostream>

using namespace std;

class Question
{
private:
	string _question;
	string* _answers;
	int _correctAnswerIndex;
	int _id;

public:
	Question(int, string, string, string, string, string);

	string getQuestion();
	string* getAnswers();
	int getCorrectAnswerIndex();
	int getId();
};