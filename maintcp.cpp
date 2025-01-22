#include <iostream>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#define IP "192.168.0.199"
#define BUFLEN 512
#define PORT 34933
int main()
{
    WSADATA ws;
    std::cout << "Initialising Winsock...\n";
    if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
        std::cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Initialised.\n";

    SOCKET tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock == INVALID_SOCKET) {
        std::cout << "Could not create TCP socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    char buf[BUFLEN];
    SOCKADDR_IN serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  // IPv4
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    serv_addr.sin_port = htons(PORT);

    if (connect(tcp_sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        std::cout << "Connect failed with error code: " << WSAGetLastError() << "\n";
        closesocket(tcp_sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    std::cout << "Connected to TCP server.\n";
    // Receive welcome message
    int recv_len = recv(tcp_sock, buf, BUFLEN, 0);
    if (recv_len == SOCKET_ERROR) {
        std::cout << "recv() failed with error code: " << WSAGetLastError() << "\n";
        closesocket(tcp_sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    buf[recv_len] = '\0';
    std::cout << "Server: " << buf << "\n";

    while (true) {
        std::string message;
        std::cout << "Enter message: ";
        std::getline(std::cin, message);
        message += '\0';

        if (send(tcp_sock, message.c_str(), message.length() + 1, 0) == SOCKET_ERROR) {
            std::cout << "send() failed with error code: " << WSAGetLastError() << "\n";
            break;
        }

        recv_len = recv(tcp_sock, buf, BUFLEN, 0);
        if (recv_len == SOCKET_ERROR) {
            std::cout << "recv() failed with error code: " << WSAGetLastError() << "\n";
            break;
        }

        buf[recv_len] = '\0';
        std::cout << "Server: " << buf << "\n";
    }

    closesocket(tcp_sock);
    WSACleanup();
    return 0;

}
