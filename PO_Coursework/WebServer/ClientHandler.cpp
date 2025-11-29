// ClientHandler.cpp
#include "ClientHandler.h"
#include <iostream>

ClientHandler::ClientHandler(InvertedIndex& idx) : index(idx) {}

void ClientHandler::handle() {
    std::cout << "Client handler started (Logic to be implemented)\n";
}