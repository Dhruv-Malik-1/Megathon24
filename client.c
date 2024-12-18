#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "networking.h"

#define SERVER_IP "10.42.0.1"  // Replace with your server's IP address
#define PORT 8080
#define ROWS 10
#define COLS 10
#define BUFFER_SIZE 1024

char** map;
char* name;

// Function to receive messages (map) from the server
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
        printf("\nUpdated Map:\n%s\n", buffer); // Print the updated map
    }

    close_socket(sockfd); // Close the socket
    return NULL;
}

int init_connect () {
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

    printf("Connected to the server.\n");
    return sockfd;
} 

int main() {
    // Getting the name of the player
    name = (char*)calloc(100, sizeof(char));
    printf("Enter your name: ");
    scanf("%[^\n]", name);
    printf("You entered: %s\n", name);

    // Connect the client to the server
    int sockfd = init_connect();

    // Initialize the grid
    map = (char**)calloc(ROWS, sizeof(char*));
    for (int i = 0; i < ROWS; i++) {
        map[i] = (char*)calloc(COLS, sizeof(char));
    }

    // Start receiving messages (the map) from the server
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, receive_messages, &sockfd); 

    // Send movement commands
    char command[2];
    while (1) {
        printf("Enter command (W/A/S/D or 'exit' to quit): ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0; // Remove newline character

        if (strcmp(command, "exit") == 0) {
            break; // Exit the loop
        }

        send_data(sockfd, command, strlen(command)); // Send command to the server
    }

    free(map); // Free allocated memory
    close_socket(sockfd); // Close the socket
    return 0;
}
