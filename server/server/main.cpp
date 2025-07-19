#include "server.h"
#include <iostream>

int main() {
    WinSocketServer server("127.0.0.1", 8080);

    if (!server.Start()) {
        return 1;
    }

    if (server.AcceptConnection()) {
        char buffer[1024];
        int bytesReceived = server.ReceiveData(buffer, sizeof(buffer));
        if (bytesReceived > 0) {
            std::cout << "Received: " << std::string(buffer, bytesReceived) << std::endl;
            server.SendData("Hello from server!", 18);
        }
    }

    server.Stop();
    return 0;
}