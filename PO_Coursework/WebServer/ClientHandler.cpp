// ClientHandler.cpp
#include "ClientHandler.h"
#include <iostream>

ClientHandler::ClientHandler(SOCKET socket, InvertedIndex& idx)
    : clientSocket(socket), index(idx) {
}

void ClientHandler::sendInt(int value) {
    // Відправляємо int як 4 байти
    send(clientSocket, (char*)&value, sizeof(value), 0);
}

void ClientHandler::sendString(const std::string& str) {
    // Спочатку довжину, потім сам рядок
    int len = str.length();
    sendInt(len);
    send(clientSocket, str.c_str(), len, 0);
}

void ClientHandler::handle() {
    // 1. Читаємо команду (1 байт)
    uint8_t command;
    int bytesReceived = recv(clientSocket, (char*)&command, sizeof(command), 0);

    if (bytesReceived <= 0) {
        closesocket(clientSocket);
        return;
    }

    if (command == CMD_SEARCH) {
        // 2. Читаємо довжину слова (4 байти)
        int wordLen;
        recv(clientSocket, (char*)&wordLen, sizeof(wordLen), 0);

        // 3. Читаємо саме слово
        std::vector<char> buffer(wordLen + 1); // +1 для \0 про всяк випадок
        recv(clientSocket, buffer.data(), wordLen, 0);
        buffer[wordLen] = '\0'; // Робимо C-string

        std::string searchWord(buffer.data());

        int filesDone = index.getFilesCount();
        std::cout << "[SERVER] Client requested search for: " << searchWord << "' "
            << "(Files indexed so far: " << filesDone << ")\n";

        // 4. Виконуємо пошук
        // Використовуємо наш метод очистки, щоб формат співпадав з індексом
        std::vector<std::string> results = index.search(InvertedIndex::cleanWord(searchWord));

        // 5. Формуємо відповідь
        if (results.empty()) {
            uint8_t status = RESP_EMPTY;
            send(clientSocket, (char*)&status, sizeof(status), 0);

            int count = 0;
            sendInt(count);
        }
        else {
            uint8_t status = RESP_OK;
            send(clientSocket, (char*)&status, sizeof(status), 0);

            int count = results.size();
            sendInt(count);

            for (const auto& docName : results) {
                sendString(docName);
            }
        }
    }
    else {
        // Невідома команда
        uint8_t status = RESP_ERROR;
        send(clientSocket, (char*)&status, sizeof(status), 0);
    }

    // Закриваємо з'єднання після обробки (для простого протоколу це ок)
    closesocket(clientSocket);
}