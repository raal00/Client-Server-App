#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <ctime>
#include <vector>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include "Logger.h"

#define _WIN32_WINNT 0x501			// freeaddrinfo bind
#pragma comment(lib, "Ws2_32.lib")	// link dll to proj

#define MAXCONNECTIONS 4
int connections = 0;
vector<int> clientSockets;

using namespace std;

void listenClient(int client_socket);
bool isSocketExist(int socket);
int getConnectionCount(vector<int> sockets);

int main(void)
{
	setlocale(LC_ALL, "");

	printInput("������� ����");
	char* port;
	port = new char[10];
	cin >> port;
	bool isServerWorking = true;
	
	connections = 0;
	printToConsoleSetting("��������� �������..");
	WSAData wsaData; 
	
	printToConsoleSetting("Loading socket dll..");
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	if (res != 0)
	{
		printToConsoleError("�� ������� ��������� socket dll!");
		return -1;
	}
	printToConsoleSuccess("�������� socket dll ��������� �������!");

	addrinfo* address = NULL; 
	printToConsoleSetting("��������� ������..");
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	res = getaddrinfo("0.0.0.0", port, &hints, &address);

	if (res != 0) 
	{
		printToConsoleError("�� ������������������ ������ ������");
		WSACleanup();
		return -2;
	}
	printToConsoleSuccess("����� �������� �������!");
	printToConsoleSetting("�������� ������ �������������..");

	int listen_socket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		cout << WSAGetLastError() << endl;
		freeaddrinfo(address);
		WSACleanup();
		return -3;
	}
	printToConsoleSuccess("�������� ������ ������������� ������� ���������!");
	printToConsoleSetting("��������� ������ �������������..");
	res = bind(listen_socket, address->ai_addr, (int)address->ai_addrlen);

	if (res == SOCKET_ERROR) // 
	{
		printToConsoleError("�� ������� ��������� ����� � ������");
		freeaddrinfo(address);
		closesocket(listen_socket);
		WSACleanup();
		return -4;
	}
	printToConsoleSuccess("��������� ������ ������������� ������� ���������!");
	printToConsoleSuccess("��������� ������� ������� ���������!");
	int max_clients = MAXCONNECTIONS;

	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &address, str, INET_ADDRSTRLEN);
	printLine();
	string hostAddress(str);
	string link = "����� �������:" + hostAddress + ":" + port + "/";
	printToConsoleInfo(link);
	printLine();

	printToConsoleSetting("�������� �����������..");
	res = listen(listen_socket, max_clients);
	if (res == SOCKET_ERROR)
	{
		printToConsoleError("������ ������");
		return -5;
	}

	while (isServerWorking)
	{
		printLine();
		connections = getConnectionCount(clientSockets);
		while (connections >= MAXCONNECTIONS) {
			printToConsoleWarning("���������� �������� ����� ����������. ������� ����� �����������!");
			Sleep(1000);
		}
		printToConsoleSetting("��������� ������ �����������..");
		int client_socket = accept(listen_socket, NULL, NULL);
		if (client_socket == INVALID_SOCKET)
		{
			printToConsoleError("������ ������");
			closesocket(listen_socket);
			WSACleanup();
			return -6;
		}
		const char* connectionResponse = "����������� � ������� ������� ���������";
		clientSockets.push_back(client_socket);
		send(client_socket, connectionResponse, 40, 0);
		printToConsoleSuccess("����������� ������� ��������!");
		thread clientThread(listenClient, client_socket);
		clientThread.detach();
	}
	printToConsoleSetting("���������� �������������..");
	closesocket(listen_socket);
	freeaddrinfo(address);
	WSACleanup();
	printToConsoleSuccess("���������� ������������� ������� ���������!");
	printLine();
	return 0;
}


void listenClient(int client_socket) 
{
	Sleep(10);
	bool isConnecting = true;

	while (isConnecting) {
		printToConsoleSetting("��������� ������ �������..");
		const int max_buffer_size = 1024;
		char buffer[max_buffer_size];
		int res = recv(client_socket, buffer, max_buffer_size, 0);

		printToConsoleSuccess("����� ������� �������!");

		if (res < 1024) buffer[res] = '\0';
		else buffer[1023] = '\0';

		string request(buffer);
		printToConsoleInfo("������ ������������: " + request);

		printToConsoleSetting("�������� ������..");
		stringstream response;
		stringstream response_body;

		if (res == SOCKET_ERROR)
		{
			printToConsoleError("������ ������");
			isConnecting = false;
			closesocket(client_socket);
		}
		else if (res == 0)
		{
			isConnecting = false;
			printToConsoleError("������ ������");
		}
		else if (res > 0)
		{
			if (request[0] == '#') {
				if (request == "#time") {
					time_t now = time(0);
					char* date_time = ctime(&now);
					response 
						<< "{" << endl 
						<< "\t" << date_time
						<< "}";
				}
				else if (request == "#close") {
					response 
						<< "{" << endl 
						<< "\t�� ���� ��������� �� �������" << endl
						<< "}";
					isConnecting = false;
				}
				else if (request == "#users") {
					connections = getConnectionCount(clientSockets);
					response 
						<< "{" << endl 
						<< "\t������� ���������� �������������: " << connections << endl
						<< "}";
				}
				else {
					response << "{ ������ ������� }"  << endl;
				}
			}
			else {
				int length = request.length();
				char* result = new char[length + 1];
				for (int i = 0; i < length; i++) {
					result[i] = request[length - 1 - i];
				}
				result[length] = '\0';
				response 
					<< "{" << endl
					<< "\t����� �������: " << request.length() << endl
					<< "\t������: " << request << endl
					<< "\t���������: " << result << endl
					<< "}";
			}
			res = send(client_socket, response.str().c_str(), response.str().length(), 0);

			if (res == SOCKET_ERROR || !isConnecting)
			{
				connections--;
				closesocket(client_socket);
			}
			printToConsoleSuccess("�������� ���������");
		}
	}
}

int getConnectionCount(vector<int> sockets) {
	int connections = 0;
	for (int socket : sockets)
	{
		int error_code;
		int error_code_size = sizeof(error_code);
		int result = getsockopt(socket, SOL_SOCKET, SO_ERROR, (char*)&error_code, &error_code_size);
		if (result == 0) {
			connections++;
		}
	}
	printToConsoleInfo("������� ����������: " + to_string(connections));
	return connections;
}