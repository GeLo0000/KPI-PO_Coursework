// WebServer.cpp
#include "WebServer.h"
#include "ClientHandler.h"

WebServer::WebServer(int port, InvertedIndex& idx, ThreadPool& tp)
    : port(port), index(idx), pool(tp), isRunning(false), serverSocket(INVALID_SOCKET) {
}

WebServer::~WebServer() {
    stop();
}

void WebServer::start() {
    // 1. Ініціалізація Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return;
    }

    // 2. Створення сокета
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return;
    }

    // 3. Налаштування адреси (IP та Порт)
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Слухаємо на всіх інтерфейсах
    serverAddr.sin_port = htons(port);       // Порт (htons перетворює число в мережевий формат)

    // 4. Прив'язка (Bind)
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed.\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    // 5. Прослуховування (Listen)
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed.\n";
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    isRunning = true;
    std::cout << "Server started on port " << port << ". Waiting for connections...\n";

    // 6. Головний цикл прийому клієнтів
    while (isRunning) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            if (isRunning) std::cerr << "Accept failed.\n";
            continue;
        }

        // Клієнт підключився!
        // Передаємо обробку клієнта в Пул Потоків
        pool.enqueue([this, clientSocket]() {
            ClientHandler handler(clientSocket, this->index);
            handler.handle();
            });
    }
}

void WebServer::stop() {
    isRunning = false;
    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
    }
    WSACleanup();
}