#include "stdafx.h"
#pragma comment(lib,"ws2_32.lib")
#include<WinSock2.h>
#include<iostream>
#include<vector>
#include<map>
#include<algorithm>
#include<string>
#pragma warning(disable: 4996)

SOCKET Connections[100];
int counter = 0;

bool space(char c)
{
	return isspace(c);
}

bool not_space(char c)
{
	return !isspace(c);
}

std::vector<std::string> split(const std::string&str)
{
	typedef std::string::const_iterator iter;
	std::vector<std::string> ret;
	iter i = str.begin();
	while (i != str.end())
	{
		i = find_if(i, str.end(), not_space);
		iter j = find_if(i, str.end(), space);
		if (i != str.end())
		{
			ret.push_back(std::string(i, j));
		}
		i = j;
	}
	return ret;
}

std::string word_count(const char*str)
{
	std::string msg_str(str),answer_to_client; 
	std::map<std::string, int> result;
	std::vector<std::string> words;
	words = split(msg_str);
	for (auto it = words.begin(); it != words.end(); ++it)
		++result[*it];
	for (auto it = words.begin(); it != words.end(); ++it)
		answer_to_client += *it + "  " + std::to_string(result[*it]) + '\n';
	
	
	return answer_to_client;
}


void clientHandler(int index)
{
	
	int msg_size;
	while (true)
	{
		recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
		char*msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connections[index], msg, msg_size, NULL);
		for (size_t i = 0; i < counter; i++)
		{
			if (i == index)
			{
				std::string answer = word_count(msg);
				int size_answer = answer.size();
				send(Connections[i], (char*)&size_answer, sizeof(int), NULL);
				send(Connections[i], answer.c_str(), size_answer, NULL);
			}
		}
		delete[]msg;
	}
}

int main(int argc, char*argv[])
{
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		std::cout << "error " << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int	sizeOfAddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	

	for (size_t i = 0; i < 100; i++)
	{
		SOCKET newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeOfAddr);
		if (newConnection == 0)
		{
			std::cout << "Error #2\n";
		}
		else
		{
			std::cout << "Client is connected!\n";
			Connections[i] = newConnection;
			counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, (LPVOID)(i), NULL, NULL);

		}
	}

	system("pause");
	return 0;
}



