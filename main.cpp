#include <iostream>
#include <winsock2.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

#define BROADCAST_IP "255.255.255.255"  // Broadcast IP address
#define BUFLEN 512  // Max length of answer
#define PORT_IN 20001  // Port for incoming data
#define PORT_OUT 20000  // Port for outgoing data

class UDPClient {
public:
    UDPClient() {
        // Initialise winsock
        cout << "Initialising Winsock...\n";
        if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
            cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
        cout << "Initialised.\n";

        // Create socket for sending
        if ((send_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
            cout << "socket() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        // Enable broadcast on send socket
        char broadcast = '1';
        if (setsockopt(send_socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0) {
            cout << "setsockopt() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        // Setup address structure for sending
        memset((char*)&send_addr, 0, sizeof(send_addr));
        send_addr.sin_family = AF_INET;
        send_addr.sin_port = htons(PORT_OUT);
        send_addr.sin_addr.S_un.S_addr = inet_addr(BROADCAST_IP);

        // Create socket for receiving
        if ((recv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
            cout << "socket() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        // Setup address structure for receiving
        memset((char*)&recv_addr, 0, sizeof(recv_addr));
        recv_addr.sin_family = AF_INET;
        recv_addr.sin_port = htons(PORT_IN);
        recv_addr.sin_addr.S_un.S_addr = INADDR_ANY;

        // Bind receiving socket
        if (bind(recv_socket, (sockaddr*)&recv_addr, sizeof(recv_addr)) == SOCKET_ERROR) {
            cout << "bind() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }
    }

    ~UDPClient() {
        closesocket(send_socket);
        closesocket(recv_socket);
        WSACleanup();
    }

    void start() {
        while (true) {
            char message[BUFLEN];
            cout << "Enter message: ";
            cin.getline(message, BUFLEN);

            // Send the message
            if (sendto(send_socket, message, strlen(message), 0, (sockaddr*)&send_addr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
                cout << "sendto() failed with error code: " << WSAGetLastError() << "\n";
                exit(EXIT_FAILURE);
            }

            char buffer[BUFLEN] = {};
            sockaddr_in sender;
            int sender_len = sizeof(sender);

            // Receive a broadcast message
            int recv_len;
            if ((recv_len = recvfrom(recv_socket, buffer, BUFLEN, 0, (sockaddr*)&sender, &sender_len)) == SOCKET_ERROR) {
                cout << "recvfrom() failed with error code: " << WSAGetLastError() << "\n";
                exit(EXIT_FAILURE);
            }

            cout << "Received message: " << buffer << "\n";
        }
    }

private:
    WSADATA ws;
    SOCKET send_socket;
    SOCKET recv_socket;
    sockaddr_in send_addr;
    sockaddr_in recv_addr;
};

int main() {
    system("title UDP Client");

    UDPClient udpClient;
    udpClient.start();
}