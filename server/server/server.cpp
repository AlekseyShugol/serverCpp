#pragma comment(lib, "ws2_32.lib")// TODO: find out why it suddenly became mandatory (maybe it was like that only on WIN systems?, it is not required under UNIX)

#include "server.h"
#include <iostream>

/*
I don't like this initialization inside the constructor, but it is more efficient and faster;
REWRITE TO CLASSIC INITIALIZATION!!!
*/
WinSocketServer::WinSocketServer(const std::string& host, int port)
    : m_host(host), m_port(port), m_listenSocket(INVALID_SOCKET),
    m_clientSocket(INVALID_SOCKET), m_isRunning(false) {


    if (WSAStartup(MAKEWORD(2, 2), &m_wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
    }
}

WinSocketServer::~WinSocketServer() {
    Stop();
    WSACleanup();
}

bool WinSocketServer::Start() {
    m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSocket == INVALID_SOCKET) {
        std::cerr << "socket failed: " << WSAGetLastError() << std::endl;
        return false;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(m_host.c_str());
    serverAddr.sin_port = htons(m_port);

    if (bind(m_listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "bind failed: " << WSAGetLastError() << std::endl;
        Cleanup();
        return false;
    }

    if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen failed: " << WSAGetLastError() << std::endl;
        Cleanup();
        return false;
    }

    m_isRunning = true;
    std::cout << "Server started on " << m_host << ":" << m_port << std::endl;
    return true;
}

void WinSocketServer::Stop() {
    if (m_isRunning) {
        Cleanup();
        m_isRunning = false;
        std::cout << "Server stopped" << std::endl;
    }
}

bool WinSocketServer::AcceptConnection() {
    if (!m_isRunning)
    {
        std::cerr << "[ERROR] Server is not running, cannot accept connections" << std::endl;
        return false;
    }

    m_clientSocket = accept(m_listenSocket, NULL, NULL);
    if (m_clientSocket == INVALID_SOCKET) {
        std::cerr << "accept failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    sockaddr_in client_addr;
    int addr_len = sizeof(client_addr);
    getpeername(m_clientSocket, (sockaddr*)&client_addr, &addr_len);

    // Выводим более информативное сообщение
    std::cout << "[INFO] Client connected from "
        << inet_ntoa(client_addr.sin_addr) << ":"
        << ntohs(client_addr.sin_port) << std::endl;
    std::cout << "Client connected" << std::endl;

    return true;
}

int WinSocketServer::ReceiveData(char* buffer, int bufferSize) {
    if (m_clientSocket == INVALID_SOCKET) return -1;

    int bytesReceived = recv(m_clientSocket, buffer, bufferSize, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        return -1;
    }

    return bytesReceived;
}

bool WinSocketServer::SendData(const char* data, int length) {
    if (m_clientSocket == INVALID_SOCKET) return false;

    int bytesSent = send(m_clientSocket, data, length, 0);
    if (bytesSent == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        return false;
    }

    return true;
}

void WinSocketServer::Cleanup() {
    if (m_clientSocket != INVALID_SOCKET) {
        closesocket(m_clientSocket);
        m_clientSocket = INVALID_SOCKET;
    }

    if (m_listenSocket != INVALID_SOCKET) {
        closesocket(m_listenSocket);
        m_listenSocket = INVALID_SOCKET;
    }
}