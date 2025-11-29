// WebServer.cpp
#include "WebServer.h"
#include "ClientHandler.h"
#include <iostream>

WebServer::WebServer(int port, InvertedIndex& idx, ThreadPool& tp)
    : port(port), index(idx), pool(tp), isRunning(false) {
}

void WebServer::start() {
    std::cout << "Server started on port " << port << " (Logic to be implemented)\n";
    isRunning = true;

    // Тут буде нескінченний цикл accept()
    // while (isRunning) { ... }
}

void WebServer::stop() {
    isRunning = false;
}