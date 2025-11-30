// ClientHandler.h
#pragma once
#include <winsock2.h>
#include "InvertedIndex.h"
#include "Protocol.h"
#include "ThreadPool.h" // Тепер ми знаємо про пул

class ClientHandler {
private:
    SOCKET clientSocket;
    InvertedIndex& index;
    ThreadPool& indexingPool; // Посилання на пул індексації (для статистики)

    void sendInt(int value);
    void sendString(const std::string& str);

public:
    // Оновили конструктор
    ClientHandler(SOCKET socket, InvertedIndex& idx, ThreadPool& iPool);
    void handle();
};