#include "ClientHandler.h"
#include <iostream>

ClientHandler::ClientHandler(SOCKET socket, InvertedIndex& idx, ThreadPool& i_pool)
    : client_socket_(socket), index_(idx), indexing_pool_(i_pool) {
}

void ClientHandler::SendInt(int value) {
    send(client_socket_, (char*)&value, sizeof(value), 0);
}

void ClientHandler::SendString(const std::string& str) {
    int len = static_cast<int>(str.length());
    SendInt(len);
    send(client_socket_, str.c_str(), len, 0);
}

bool ClientHandler::RecvAll(char* buffer, int length) {
    int total_received = 0;
    while (total_received < length) {
        int bytes = recv(client_socket_, buffer + total_received, length - total_received, 0);

        if (bytes <= 0) {
            return false; // Error or connection closed
        }
        total_received += bytes;
    }
    return true;
}

void ClientHandler::Handle() {
    // 1. Read Command (1 byte)
    uint8_t command;
    if (!RecvAll((char*)&command, sizeof(command))) {
        closesocket(client_socket_);
        return;
    }

    if (command == kCmdSearch) {
        // 2. Read Word Length (4 bytes)
        int word_len;
        if (!RecvAll((char*)&word_len, sizeof(word_len))) {
            closesocket(client_socket_);
            return;
        }

        // 3. Read Word
        std::vector<char> buffer(word_len + 1);
        if (!RecvAll(buffer.data(), word_len)) {
            closesocket(client_socket_);
            return;
        }
        buffer[word_len] = '\0';

        std::string search_word(buffer.data());

        // --- Stats Output ---
        int processed = index_.GetFilesCount();
        size_t in_queue = indexing_pool_.GetQueueSize();

        std::cout << "[SERVER] Search: '" << search_word << "' | "
            << "Processed: " << processed << " | "
            << "Queue: " << in_queue << "\n";
        // --------------------

        // 4. Perform Search (using CleanWord and Search methods)
        std::vector<std::string> results = index_.Search(InvertedIndex::CleanWord(search_word));

        // 5. Send Response
        if (results.empty()) {
            uint8_t status = kRespEmpty;
            send(client_socket_, (char*)&status, sizeof(status), 0);
        }
        else {
            uint8_t status = kRespOk;
            send(client_socket_, (char*)&status, sizeof(status), 0);

            int count = static_cast<int>(results.size());
            SendInt(count);

            for (const auto& doc_name : results) {
                SendString(doc_name);
            }
        }
    }
    else {
        // Unknown command
        uint8_t status = kRespError;
        send(client_socket_, (char*)&status, sizeof(status), 0);
    }

    std::cout << "[SERVER] << Client disconnected.\n";
    closesocket(client_socket_);
}