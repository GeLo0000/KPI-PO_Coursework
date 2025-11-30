#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include "InvertedIndex.h"
#include "Protocol.h"
#include "ThreadPool.h"

// Handles communication with a single connected client.
class ClientHandler {
public:
    // Constructs the handler.
    ClientHandler(SOCKET socket, InvertedIndex& idx, ThreadPool& i_pool);

    // Main loop to process client requests.
    void Handle();

private:
    // Sends a 4-byte integer to the client.
    void SendInt(int value);

    // Sends a string (length + data) to the client.
    void SendString(const std::string& str);

    // Guarantees reading of exact number of bytes.
    bool RecvAll(char* buffer, int length);

    SOCKET client_socket_;
    InvertedIndex& index_;
    ThreadPool& indexing_pool_;
};