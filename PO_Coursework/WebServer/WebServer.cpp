#include "WebServer.h"
#include "ClientHandler.h"

WebServer::WebServer(int port, InvertedIndex& idx, ThreadPool& c_pool, ThreadPool& i_pool)
    : port_(port), index_(idx), client_pool_(c_pool), indexing_pool_(i_pool),
    is_running_(false), server_socket_(INVALID_SOCKET) {
}

WebServer::~WebServer() {
    Stop();
}

void WebServer::Start() {
    // 1. Initialize Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return;
    }

    // 2. Create Socket
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return;
    }

    // 3. Bind
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    if (bind(server_socket_, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed.\n";
        closesocket(server_socket_);
        WSACleanup();
        return;
    }

    // 4. Listen
    if (listen(server_socket_, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed.\n";
        closesocket(server_socket_);
        WSACleanup();
        return;
    }

    is_running_ = true;
    std::cout << "Server started on port " << port_ << ". Waiting for connections...\n";

    // 5. Accept Loop
    while (is_running_) {
        SOCKET client_socket = accept(server_socket_, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            if (is_running_) std::cerr << "Accept failed.\n";
            continue;
        }

        std::cout << "[SERVER] >> New client connected.\n";

        // Dispatch to client thread pool
        client_pool_.Enqueue([this, client_socket]() {
            ClientHandler handler(client_socket, this->index_, this->indexing_pool_);
            handler.Handle();
            });
    }
}

void WebServer::Stop() {
    is_running_ = false;
    if (server_socket_ != INVALID_SOCKET) {
        closesocket(server_socket_);
    }
    WSACleanup();
}