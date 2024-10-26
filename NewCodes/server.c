#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "networking.h"
#include "graphic.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int clients[MAX_CLIENTS]; // Array to keep track of client sockets
int client_count = 0; // Number of connected clients
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for thread safety

char map[10][10]; // 10x10 map
int positions[MAX_CLIENTS][2]; // Store x, y positions for each client

// Function to initialize the map with zeros
void init_map() {
    memset(map, ' ', sizeof(map)); // Fill the map with spaces
    for (int i = 0; i < MAX_CLIENTS; i++) {
        positions[i][0] = 0; // Initialize x position
        positions[i][1] = 0; // Initialize y position
    }
}

// Function to broadcast the updated map to all clients
void broadcast_map() {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer)); // Clear the buffer

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            buffer[i * 11 + j] = map[i][j]; // Add map to the buffer
            if (j < 9) buffer[i * 11 + j + 1] = ' '; // Add space between cells
        }
        buffer[(i + 1) * 11 - 1] = '\n'; // New line after each row
    }

    pthread_mutex_lock(&client_mutex); // Lock mutex
    for (int i = 0; i < client_count; i++) {
        send_data(clients[i], buffer, strlen(buffer)); // Send the map to each client
    }
    pthread_mutex_unlock(&client_mutex); // Unlock mutex
}

// Function to handle each client connection
void *client_handler(void *arg) {
    int client_sock = *(int *)arg;
    char buffer[BUFFER_SIZE];

    // Initialize map and positions
    init_map();
    broadcast_map(); // Send the initial map to the client

    while (1) {
        ssize_t bytes_received = receive_data(client_sock, buffer, sizeof(buffer) - 1);
        if (bytes_received <= 0) {
            // Client disconnected
            break;
        }

        buffer[bytes_received] = '\0'; // Null-terminate the string
        //printf("Client says: %s\n", buffer);

        // Update position based on command
        if (strcmp(buffer, "W") == 0) {
            positions[client_sock][0] = (positions[client_sock][0] - 1 + 10) % 10; // Move up
        } else if (strcmp(buffer, "A") == 0) {
            positions[client_sock][1] = (positions[client_sock][1] - 1 + 10) % 10; // Move left
        } else if (strcmp(buffer, "S") == 0) {
            positions[client_sock][0] = (positions[client_sock][0] + 1) % 10; // Move down
        } else if (strcmp(buffer, "D") == 0) {
            positions[client_sock][1] = (positions[client_sock][1] + 1) % 10; // Move right
        }

        // Clear the map and update it with the new positions
        memset(map, ' ', sizeof(map)); // Clear the map
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (positions[i][0] >= 0 && positions[i][1] >= 0) {
                map[positions[i][0]][positions[i][1]] = '#'; // Mark position with '#'
            }
        }

        // Broadcast the updated map to all clients
        broadcast_map();
    }

    // Remove the client from the list and notify others
    pthread_mutex_lock(&client_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == client_sock) {
            clients[i] = clients[--client_count]; // Remove the client
            break;
        }
    }
    pthread_mutex_unlock(&client_mutex);
    close_socket(client_sock); // Close the client socket
    return NULL;
}

int main() {
    int server_fd = create_socket();
    
    // Enable SO_REUSEADDR to allow immediate reuse of the port
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // Accept connections on any IP
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Connected to client: %s\n", inet_ntoa(client_addr.sin_addr));

        // Add the new client to the clients array and start a new thread for it
        pthread_mutex_lock(&client_mutex);
        if (client_count < MAX_CLIENTS) {
            clients[client_count++] = client_fd;
            pthread_t tid;
            pthread_create(&tid, NULL, client_handler, &client_fd);
        } else {
            printf("Max clients connected. Rejecting new connection.\n");
            close_socket(client_fd); // Reject new client
        }
        pthread_mutex_unlock(&client_mutex);
    }

    close_socket(server_fd); // Close the server socket
    return 0;
}
