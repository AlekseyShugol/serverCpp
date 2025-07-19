#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <string>

class WinSocketServer {
public:
    WinSocketServer(const std::string& host, int port);
    ~WinSocketServer();

    bool Start();
    void Stop();
    bool AcceptConnection();
    int ReceiveData(char* buffer, int bufferSize);
    bool SendData(const char* data, int length);

private:
    std::string m_host;
    int m_port;
    SOCKET m_listenSocket;
    SOCKET m_clientSocket;
    WSADATA m_wsaData;
    bool m_isRunning;

    void Cleanup();
};