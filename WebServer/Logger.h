#pragma once
#include <iostream>
#include <string>

using namespace std;

void printToConsoleError(string message);
void printToConsoleWarning(string message);
void printToConsoleSuccess(string message);
void printToConsoleInfo(string message);
void printToConsoleSetting(string message);

void printInput(string message);

void printLine();

void writeToFile(string message);