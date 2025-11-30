#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include "InvertedIndex.h"
#include "ThreadPool.h"

// Link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")

// Main server class handling incoming TCP connections.
class WebServer {
public:
    // Constructs the WebServer.
    WebServer(int port, InvertedIndex& idx, ThreadPool& c_pool, ThreadPool& i_pool);

    // Destructor. Stops the server.
    ~WebServer();

    // Starts listening for connections. Blocking call.
    void Start();

    // Stops the server and closes the socket.
    void Stop();

private:
    int port_;
    InvertedIndex& index_;
    ThreadPool& client_pool_;
    ThreadPool& indexing_pool_;

    bool is_running_;
    SOCKET server_socket_;
};