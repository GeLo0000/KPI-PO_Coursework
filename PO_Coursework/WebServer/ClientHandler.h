#pragma once
#include <winsock2.h>
#include <vector>
#include <string>
#include "InvertedIndex.h"
#include "Protocol.h" // Наш протокол

class ClientHandler {
private:
    SOCKET clientSocket;
    InvertedIndex& index;

    // Допоміжні методи для відправки чисел
    void sendInt(int value);
    void sendString(const std::string& str);

public:
    ClientHandler(SOCKET socket, InvertedIndex& idx);
    void handle();
};