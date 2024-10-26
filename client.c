#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

void *receive_messages(void *socket) {
    int sock = *(int *)socket;
    char buffer[1024];

    while (1) {
        memset(buffer, 0, sizeof(buffer));  // Clear buffer
        int valread = read(sock, buffer, sizeof(buffer));
        if (valread <= 0) {
            printf("Server disconnected.\n");
            break;  // Exit the loop if the server is disconnected
        }
        printf("%s", buffer);  // Print the received message
    }
    return NULL;
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }

    // Setting up the server address structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert the IP address from text to binary form
    if (inet_pton(AF_INET, "10.2.139.125", &serv_addr.sin_addr) <= 0) {  // Replace with server's IP
        printf("Invalid address/ Address not supported\n");
        return -1;
    }

    // Connecting to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed\n");
        return -1;
    }
    printf("Connected to the server\n");

    // Create a thread to receive messages from the server
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_messages, (void *)&sock);

    // Main loop to send messages to the server
    while (1) {
        // Get input from the user
        printf("Enter message to send (or just press Enter to disconnect): ");
        fgets(buffer, sizeof(buffer), stdin);

        // Check if the user just pressed Enter
        if (strcmp(buffer, "\n") == 0) {
            break;  // Exit the loop and close the connection
        }

        // Send the message to the server
        send(sock, buffer, strlen(buffer), 0);
        printf("Message sent to the server\n");
    }

    // Notify server of disconnection
    char *disconnect_message = "Client is disconnecting.\n";
    send(sock, disconnect_message, strlen(disconnect_message), 0);

    // Closing the socket
    close(sock);
    printf("Disconnected from the server\n");
    return 0;
}
