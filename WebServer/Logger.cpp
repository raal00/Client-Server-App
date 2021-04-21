#include "Logger.h"
#include <Windows.h>

void printToConsoleError(string message) {
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 4);

	cout << "[ERROR]\t" << message << endl;
	SetConsoleTextAttribute(hConsole, 15);
}

void printInput(string message) {
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 3);
	cout << "[INPUT]\t\t" << message << " > ";
	SetConsoleTextAttribute(hConsole, 15);
}

void printToConsoleWarning(string message) {
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 6);
	cout << "[WARNING]\t" << message << endl;
	SetConsoleTextAttribute(hConsole, 15);
}
void printToConsoleSuccess(string message) {
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 2);
	cout << "[SUCCESS]\t" << message << endl;
	SetConsoleTextAttribute(hConsole, 15);
}
void printToConsoleInfo(string message) {
	cout << "[INFO]\t\t" << message << endl;
}
void printToConsoleSetting(string message) {
	static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 8);
	cout << "[SETTINGS]\t" << message << endl;
	SetConsoleTextAttribute(hConsole, 15);
}

void printLine() {
	cout << endl << "___________________________________" << endl;
}


void writeToFile(string message) {

}