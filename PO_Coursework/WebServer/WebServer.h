#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include "InvertedIndex.h"
#include "ThreadPool.h"

// Link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

/// @brief Main server class handling incoming TCP connections.
class WebServer {
public:
    /// @brief Constructs the WebServer.
    /// @param port Port to listen on.
    /// @param idx Reference to the InvertedIndex.
    /// @param c_pool Thread pool for handling clients.
    /// @param i_pool Thread pool for indexing tasks (used for stats).
    WebServer(int port, InvertedIndex& idx, ThreadPool& c_pool, ThreadPool& i_pool);

    /// @brief Destructor. Stops the server.
    ~WebServer();

    /// @brief Starts listening for connections. Blocking call.
    void Start();

    /// @brief Stops the server and closes the socket.
    void Stop();

private:
    int port_;
    InvertedIndex& index_;
    ThreadPool& client_pool_;
    ThreadPool& indexing_pool_;

    bool is_running_;
    SOCKET server_socket_;
};