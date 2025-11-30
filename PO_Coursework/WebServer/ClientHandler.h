#pragma once

#include <winsock2.h>
#include <string>
#include <vector>
#include "InvertedIndex.h"
#include "Protocol.h"
#include "ThreadPool.h"

/// @brief Handles communication with a single connected client.
class ClientHandler {
public:
    /// @brief Constructs the handler.
    /// @param socket The connected client socket.
    /// @param idx Reference to the InvertedIndex.
    /// @param i_pool Reference to the Indexing ThreadPool (for stats).
    ClientHandler(SOCKET socket, InvertedIndex& idx, ThreadPool& i_pool);

    /// @brief Main loop to process client requests.
    void Handle();

private:
    /// @brief Sends a 4-byte integer to the client.
    void SendInt(int value);

    /// @brief Sends a string (length + data) to the client.
    void SendString(const std::string& str);

    /// @brief Guarantees reading of exact number of bytes.
    /// Fixes "partial read" issues for large packets.
    /// @param buffer Destination buffer.
    /// @param length Number of bytes to read.
    /// @return True if successful, false on error.
    bool RecvAll(char* buffer, int length);

    SOCKET client_socket_;
    InvertedIndex& index_;
    ThreadPool& indexing_pool_;
};