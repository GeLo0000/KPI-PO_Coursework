// ClientHandler.h
#pragma once
// #include <winsock2.h> // Поки закоментуємо, підключимо пізніше
#include "InvertedIndex.h"

class ClientHandler {
private:
    // SOCKET clientSocket; 
    InvertedIndex& index;

public:
    ClientHandler(InvertedIndex& idx); // Тут буде socket в конструкторі
    void handle(); // Основний цикл обробки клієнта
};