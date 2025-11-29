// WebServer.h
#pragma once
#include <string>
#include "InvertedIndex.h"
#include "ThreadPool.h"

class WebServer {
private:
    int port;
    InvertedIndex& index;
    ThreadPool& pool;
    bool isRunning;

public:
    WebServer(int port, InvertedIndex& idx, ThreadPool& tp);
    void start();
    void stop();
};