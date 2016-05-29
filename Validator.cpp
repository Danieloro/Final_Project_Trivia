#include "Validator.h"
#include<iostream>

using namespace std;

/*Returns 'true' if the password is valid, and 'false' otherwise*/
bool Validator::isPasswordValid(string password)
{
	bool upperCase = false, lowerCase = false, number = false, space = false;
	
	if (password.length() < 4)
		return false;

	for (int i = 0; i < password.length(); i++)
	{
		if (password[i] >= 'A' && password[i] <= 'Z')
			upperCase = true;
		else if (password[i] >= 'a' && password[i] <= 'z')
			lowerCase = true;
		else if (password[i] == ' ')
			space = true;
		else if (password[i] >= '0' && password[i] <= '9')
			number = true;
	}

	if (upperCase && lowerCase && !space && number)
		return true;
	return false;
}

/*Returns 'true' if the username is valid, and 'false' otherwise*/
bool Validator::isUsernameValid(string username)
{
	bool space = false;
	int i = 0;

	if (username == "")
		return false;

	while (!space && i < username.length())
	{
		if (username[i] == ' ')
			space = true;
		i++;
	}

	if (!space && ((username[0] >= 'A' && username[0] <= 'Z') || (username[0] >= 'a' && username[0] <= 'z')))
		return true;
	return false;
}