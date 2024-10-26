// Command to run code  --> gcc 'Server connect test.c' -o main.exe -lws2_32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h> // Include ws2tcpip for InetPton

#pragma comment(lib, "ws2_32.lib") // Link with ws2_32.lib

#define PORT 8080

DWORD WINAPI receive_messages(LPVOID socket)
{
    int sock = *(int *)socket;
    char buffer[1024];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        int valread = recv(sock, buffer, sizeof(buffer), 0);
        if (valread <= 0)
        {
            printf("Server disconnected.\n");
            break; // Exit the loop if the server is disconnected
        }
        printf("%s", buffer); // Print the received message
    }
    return 0;
}

int main()
{
    WSADATA wsa;
    SOCKET sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return 1;
    }

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Socket creation error. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Setting up the server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert the IP address from text to binary form
    if (InetPton(AF_INET, "10.2.139.125", &serv_addr.sin_addr) <= 0)
    { // Replace with server's IP
        printf("Invalid address/ Address not supported\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Connecting to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("Connection Failed. Error Code: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("Connected to the server\n");

    // Create a thread to receive messages from the server
    HANDLE recv_thread = CreateThread(NULL, 0, receive_messages, (void *)&sock, 0, NULL);
    if (recv_thread == NULL)
    {
        printf("Thread creation failed. Error Code: %d\n", GetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Main loop to send messages to the server
    while (1)
    {
        // Get input from the user
        printf("Enter message to send (or just press Enter to disconnect): ");
        fgets(buffer, sizeof(buffer), stdin);

        // Check if the user just pressed Enter
        if (strcmp(buffer, "\n") == 0)
        {
            break; // Exit the loop and close the connection
        }

        // Send the message to the server
        send(sock, buffer, strlen(buffer), 0);
        printf("Message sent to the server\n");
    }

    // Notify server of disconnection
    char *disconnect_message = "Client is disconnecting.\n";
    send(sock, disconnect_message, strlen(disconnect_message), 0);

    // Close the socket and clean up
    closesocket(sock);
    WaitForSingleObject(recv_thread, INFINITE); // Ensure the receive thread finishes
    CloseHandle(recv_thread);
    WSACleanup();
    printf("Disconnected from the server\n");
    return 0;
}
