#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include "Protocol.h" // Update path to your Protocol.h

// Link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

// Helper function to ensure all bytes are received.
bool RecvAll(SOCKET socket, char* buffer, int length) {
    int total_received = 0;
    while (total_received < length) {
        int bytes = recv(socket, buffer + total_received, length - total_received, 0);
        if (bytes <= 0) {
            return false;
        }
        total_received += bytes;
    }
    return true;
}

int main() {
    // 1. Initialize Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    std::cout << "--- Client Started ---\n";

    while (true) {
        std::cout << "\nEnter word to search (or 'exit'): ";
        std::string word;
        std::cin >> word;

        if (word == "exit") break;

        // 2. Create Socket
        SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed.\n";
            break;
        }

        // 3. Connect to Server
        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

        if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            std::cerr << "Error: Could not connect to server. Is it running?\n";
            closesocket(client_socket);
            continue; // Try again next loop
        }

        // 4. Send Request

        // Command (1 byte)
        uint8_t cmd = kCmdSearch;
        send(client_socket, (char*)&cmd, sizeof(cmd), 0);

        // Length (4 bytes)
        int len = static_cast<int>(word.length());
        send(client_socket, (char*)&len, sizeof(len), 0);

        // Word (N bytes)
        send(client_socket, word.c_str(), len, 0);

        // 5. Receive Response

        // Status (1 byte)
        uint8_t status;
        if (RecvAll(client_socket, (char*)&status, sizeof(status))) {

            if (status == kRespOk) {
                int count;
                RecvAll(client_socket, (char*)&count, sizeof(count));

                std::cout << ">>> Found in " << count << " files:\n";

                for (int i = 0; i < count; ++i) {
                    int name_len;
                    RecvAll(client_socket, (char*)&name_len, sizeof(name_len));

                    std::vector<char> buffer(name_len + 1);
                    RecvAll(client_socket, buffer.data(), name_len);
                    buffer[name_len] = '\0';

                    std::cout << "    [" << (i + 1) << "] " << buffer.data() << "\n";
                }
            }
            else if (status == kRespEmpty) {
                std::cout << ">>> Word NOT found.\n";
            }
            else {
                std::cout << ">>> Server Error.\n";
            }
        }
        else {
            std::cout << "Error receiving data or connection closed.\n";
        }

        // Close socket after each request
        closesocket(client_socket);
    }

    WSACleanup();

    std::cout << "\nProgram finished. Press any key to exit...";
    system("pause");

    return 0;
}