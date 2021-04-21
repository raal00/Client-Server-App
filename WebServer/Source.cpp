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

	printInput("Введите порт");
	char* port;
	port = new char[10];
	cin >> port;
	bool isServerWorking = true;
	
	connections = 0;
	printToConsoleSetting("Настройка сервера..");
	WSAData wsaData; 
	
	printToConsoleSetting("Loading socket dll..");
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	if (res != 0)
	{
		printToConsoleError("Не удалось загрузить socket dll!");
		return -1;
	}
	printToConsoleSuccess("Загрузка socket dll завершена успешно!");

	addrinfo* address = NULL; 
	printToConsoleSetting("Настройка адреса..");
	addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	res = getaddrinfo("0.0.0.0", port, &hints, &address);

	if (res != 0) 
	{
		printToConsoleError("Не инициализировалась модель адреса");
		WSACleanup();
		return -2;
	}
	printToConsoleSuccess("Адрес настроен успешно!");
	printToConsoleSetting("Создание сокета прослушивания..");

	int listen_socket = socket(address->ai_family, address->ai_socktype, address->ai_protocol);
	if (listen_socket == INVALID_SOCKET)
	{
		cout << WSAGetLastError() << endl;
		freeaddrinfo(address);
		WSACleanup();
		return -3;
	}
	printToConsoleSuccess("Создание сокета прослушивания успешно завершено!");
	printToConsoleSetting("Установка сокета прослушивания..");
	res = bind(listen_socket, address->ai_addr, (int)address->ai_addrlen);

	if (res == SOCKET_ERROR) // 
	{
		printToConsoleError("Не удалось привязать адрес к сокету");
		freeaddrinfo(address);
		closesocket(listen_socket);
		WSACleanup();
		return -4;
	}
	printToConsoleSuccess("Установка сокета прослушивания успешно завершена!");
	printToConsoleSuccess("Настройка сервера успешна завершена!");
	int max_clients = MAXCONNECTIONS;

	char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &address, str, INET_ADDRSTRLEN);
	printLine();
	string hostAddress(str);
	string link = "Адрес сервера:" + hostAddress + ":" + port + "/";
	printToConsoleInfo(link);
	printLine();

	printToConsoleSetting("Ожидание подключений..");
	res = listen(listen_socket, max_clients);
	if (res == SOCKET_ERROR)
	{
		printToConsoleError("Ошибка сокета");
		return -5;
	}

	while (isServerWorking)
	{
		printLine();
		connections = getConnectionCount(clientSockets);
		while (connections >= MAXCONNECTIONS) {
			printToConsoleWarning("Невозможно получить новые соединения. Слишком много подключений!");
			Sleep(1000);
		}
		printToConsoleSetting("Получение нового подключения..");
		int client_socket = accept(listen_socket, NULL, NULL);
		if (client_socket == INVALID_SOCKET)
		{
			printToConsoleError("Ошибка сокета");
			closesocket(listen_socket);
			WSACleanup();
			return -6;
		}
		const char* connectionResponse = "Подключение к серверу успешно завершено";
		clientSockets.push_back(client_socket);
		send(client_socket, connectionResponse, 40, 0);
		printToConsoleSuccess("Подключение успешно получено!");
		thread clientThread(listenClient, client_socket);
		clientThread.detach();
	}
	printToConsoleSetting("Завершение прослушивания..");
	closesocket(listen_socket);
	freeaddrinfo(address);
	WSACleanup();
	printToConsoleSuccess("Завершение прослушивания успешно завершено!");
	printLine();
	return 0;
}


void listenClient(int client_socket) 
{
	Sleep(10);
	bool isConnecting = true;

	while (isConnecting) {
		printToConsoleSetting("Получение текста запроса..");
		const int max_buffer_size = 1024;
		char buffer[max_buffer_size];
		int res = recv(client_socket, buffer, max_buffer_size, 0);

		printToConsoleSuccess("Текст запроса получен!");

		if (res < 1024) buffer[res] = '\0';
		else buffer[1023] = '\0';

		string request(buffer);
		printToConsoleInfo("Запрос пользователя: " + request);

		printToConsoleSetting("Отправка ответа..");
		stringstream response;
		stringstream response_body;

		if (res == SOCKET_ERROR)
		{
			printToConsoleError("Ошибка сокета");
			isConnecting = false;
			closesocket(client_socket);
		}
		else if (res == 0)
		{
			isConnecting = false;
			printToConsoleError("Ошибка сокета");
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
						<< "\tВы были отключены от сервера" << endl
						<< "}";
					isConnecting = false;
				}
				else if (request == "#users") {
					connections = getConnectionCount(clientSockets);
					response 
						<< "{" << endl 
						<< "\tТекущее количество пользователей: " << connections << endl
						<< "}";
				}
				else {
					response << "{ Ошибка запроса }"  << endl;
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
					<< "\tДлина запроса: " << request.length() << endl
					<< "\tЗапрос: " << request << endl
					<< "\tРезультат: " << result << endl
					<< "}";
			}
			res = send(client_socket, response.str().c_str(), response.str().length(), 0);

			if (res == SOCKET_ERROR || !isConnecting)
			{
				connections--;
				closesocket(client_socket);
			}
			printToConsoleSuccess("Отправка завершена");
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
	printToConsoleInfo("Текущих соединений: " + to_string(connections));
	return connections;
}