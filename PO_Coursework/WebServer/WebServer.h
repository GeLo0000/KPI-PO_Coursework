#pragma once
#include <winsock2.h> // Обов'язково на початку!
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include "InvertedIndex.h"
#include "ThreadPool.h"

// Щоб VS не сварилась на старі функції
#pragma comment(lib, "ws2_32.lib")

class WebServer {
private:
    int port;
    InvertedIndex& index;
    ThreadPool& clientPool;   // Пул для клієнтів
    ThreadPool& indexingPool; // Пул для файлів (зберігаємо посилання для статистики)
    bool isRunning;
    SOCKET serverSocket; // Змінна для сокета

public:
    WebServer(int port, InvertedIndex& idx, ThreadPool& cPool, ThreadPool& iPool);
    ~WebServer(); // Деструктор для закриття сокета
    void start();
    void stop();
};