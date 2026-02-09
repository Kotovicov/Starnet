#include "Commands.h"
#include "URLEncoding.h"
#include "StringOperations.h"

#include <Windows.h>
#include <stdio.h>
#include <string>

using namespace std;

// Создает команду в формате: "ID|Тип|Закодированные_данные"
// ID=123, Тип=10 (TCPFlood), Данные="1.1.1.1@80@60"	 -->	"123|10|1.1.1.1%4080%4060"
char* CreateCommand(int commandId, int commandType, char* data, int dataLength)
{
	char* idString = (char*)malloc(11); 
	ZeroMemory(idString, 11);
	_itoa_s(commandId, idString, 11, 10); // преобразование commandId в строку idString
 	char* typeString = (char*)malloc(11);
	ZeroMemory(typeString, 11);
	_itoa_s(commandType, typeString, 11, 10); // преобразование commandType в строку typeString

	// Кодирует данные через URLEncode (заменяет спецсимволы на %XX)
	//char* encoded = URLEncode(data, dataLength); 
	char* encoded = data;
	// Объединяет всё в одну строку с разделителем |
	char** strings = new char*[5] { idString, (char*)"|", typeString, (char*)"|", encoded};
	
	int outputLen;
	char* returnBuffer = JoinString(strings, 5, &outputLen);
	
	free(idString);
	free(typeString);
	free(strings);
	
	return returnBuffer;
}

void FreeCommand(char* data)
{
	FreeJoinStringBuffer(data);
}

// Разбирает входящую команду (возвращает ID, тип и данные) 
// "123|10|1.1.1.1%4080%4060"	--->	id = 123, type = 10, data = "1.1.1.1@80@60"
int ParseCommand(char* command, char** commandData, int* commandType, char* delimetr)
{
	int numResults;
	char** splitResult = SplitString(command, delimetr, &numResults, false); // Разделяет строку по |

	if (numResults != 3) return -1; 


	int commandId = std::stoi(splitResult[0], 0, 10); // Перевод из строки в число
	*commandType = std::stoi(splitResult[1], 0, 10); // Возвращает тип команды (int)

	int commandSize = strlen(splitResult[2]);
	//char* decoded = URLDecode(splitResult[2], &commandSize); // Декодирует данные(URLDecode).
	char *decoded = splitResult[2];
	// strlen(splitResult[2]);

	
	char* commandString = (char*)malloc(commandSize + 1); 
	commandString[commandSize] = 0;
	memcpy_s(commandString, commandSize, decoded, commandSize); // копия в буфер 

	FreeSplitStringBuffer(splitResult, numResults); // освобождает splitResult
	//FreeURLDecodeResult(decoded);

	*commandData = commandString; // возвращает декодированные данные
	return commandId;
}

void FreeParsedCommandResult(char* data)
{
	free(data);
}
