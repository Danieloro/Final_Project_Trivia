#include "TriviaServer.h"
#include <iostream>

int main()
{
	TriviaServer* myServer = new TriviaServer();
	myServer->server();

	system("pause");
	return 0;
}