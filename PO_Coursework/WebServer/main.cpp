#include <iostream>
#include "InvertedIndex.h"
#include "ThreadPool.h"
#include "Indexer.h"
#include "WebServer.h"

int main() {
    // Initialize shared resources
    InvertedIndex index;

    // Create Thread Pools

    // Pool for heavy indexing tasks
    size_t indexing_threads = 20;
    ThreadPool indexing_pool(indexing_threads);

    // Pool for quick client responses
    size_t client_threads = 2;
    ThreadPool client_pool(client_threads);

    std::cout << "System initialized:\n";
    std::cout << "- Indexing Pool: " << indexing_threads << " threads\n";
    std::cout << "- Client Pool:   " << client_threads << " threads\n";

    // Start Background Indexer (non-blocking)
    Indexer indexer(index, indexing_pool, "data");
    indexer.Start();

    // Start Web Server (blocking)
    WebServer server(8080, index, client_pool, indexing_pool);
    server.Start();

    // The program will reach here only when the server stops
    indexer.Stop();

    return 0;
}