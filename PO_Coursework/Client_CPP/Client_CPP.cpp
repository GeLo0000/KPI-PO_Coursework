#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include "Protocol.h"

#pragma comment(lib, "ws2_32.lib")

int main() {
    // 1. Ініціалізація Winsock (один раз на старті)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    std::cout << "--- Client Started ---\n";

    while (true) {
        std::cout << "\nEnter word to search (or 'exit'): ";
        std::string word;
        std::cin >> word;

        if (word == "exit") break;

        // ==========================================
        // КРОК 1: Створення з'єднання (ДЛЯ КОЖНОГО ЗАПИТУ)
        // ==========================================
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed.\n";
            break;
        }

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8080);
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Error: Could not connect to server. Is it running?\n";
            closesocket(clientSocket);
            // Не виходимо, даємо шанс спробувати ще раз
            continue;
        }

        // ==========================================
        // КРОК 2: Відправка запиту
        // ==========================================

        // 1. Команда
        uint8_t cmd = CMD_SEARCH;
        send(clientSocket, (char*)&cmd, sizeof(cmd), 0);

        // 2. Довжина слова
        int len = word.length();
        send(clientSocket, (char*)&len, sizeof(len), 0);

        // 3. Саме слово
        send(clientSocket, word.c_str(), len, 0);

        // ==========================================
        // КРОК 3: Отримання відповіді
        // ==========================================

        uint8_t status;
        int bytesReceived = recv(clientSocket, (char*)&status, sizeof(status), 0);

        if (bytesReceived > 0) {
            if (status == RESP_OK) {
                int count;
                recv(clientSocket, (char*)&count, sizeof(count), 0);

                std::cout << ">>> Found in " << count << " files:\n";

                for (int i = 0; i < count; ++i) {
                    int nameLen;
                    recv(clientSocket, (char*)&nameLen, sizeof(nameLen), 0);

                    std::vector<char> buffer(nameLen + 1);
                    recv(clientSocket, buffer.data(), nameLen, 0);
                    buffer[nameLen] = '\0';

                    std::cout << "    [" << (i + 1) << "] " << buffer.data() << "\n";
                }
            }
            else if (status == RESP_EMPTY) {
                std::cout << ">>> Word NOT found.\n";
                // Вичитуємо пустий лічильник, щоб очистити сокет (протокол вимагає)
                int dummy;
                recv(clientSocket, (char*)&dummy, sizeof(dummy), 0);
            }
            else {
                std::cout << ">>> Server Error.\n";
            }
        }
        else {
            std::cout << "Error receiving data or connection closed.\n";
        }

        // Закриваємо сокет після кожного запиту, бо сервер так налаштований
        closesocket(clientSocket);
    }

    WSACleanup();

    // Ця команда не дасть вікну закритися
    std::cout << "\nProgram finished. Press any key to exit...";
    system("pause");

    return 0;
}