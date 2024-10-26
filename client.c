#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "networking.h"

#define SERVER_IP "10.42.0.1"  // Replace with your server's IP address
#define PORT 8080
#define BUFFER_SIZE 1024

void *receive_messages(void *arg) {
    int sockfd = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1) {
        ssize_t bytes_received = receive_data(sockfd, buffer, sizeof(buffer) - 1);
        if (bytes_received <= 0) {
            printf("Disconnected from server.\n");
            break;
        }
        buffer[bytes_received] = '\0'; // Null-terminate the string
        printf("Broadcast: %s\n", buffer);
    }

    close_socket(sockfd); // Close the socket
    return NULL;
}

int main() {
    int sockfd = create_socket();
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    
    // Convert SERVER_IP to binary form and set in server_addr
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Invalid address or address not supported.\n");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server. You can start chatting!\n");

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receive_messages, &sockfd); // Start receiving messages

    char message[BUFFER_SIZE];
    while (1) {
        printf("Enter message (type 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0; // Remove newline character

        if (strcmp(message, "exit") == 0) {
            break; // Exit the loop
        }

        send_data(sockfd, message, strlen(message)); // Send the message to the server
    }

    close_socket(sockfd); // Close the socket
    return 0;
}
