#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "networking.h"

#define SERVER_HOST "localhost"  // Replace with server's hostname if necessary
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
    struct hostent *server_host = gethostbyname(SERVER_HOST);
    if (server_host == NULL) {
        fprintf(stderr, "Host not found.\n");
        exit(EXIT_FAILURE);
    }

    int sockfd = create_socket();
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    memcpy(&server_addr.sin_addr.s_addr, server_host->h_addr, server_host->h_length);

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
