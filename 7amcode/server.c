#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "gamestate.h"

#define PORT 8080
#define BUFFER_SIZE 1024

Player players[MAX_PLAYERS]; // Define the players array
int player_count = 0;         // Initialize player count

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    int player_id = player_count; // Assign a new player ID

    // Assign socket to the player and inform the client of their player ID
    players[player_id].sockfd = client_sock;
    players[player_id].id = player_id + 1; // Start IDs from 1
    players[player_id].x = INITIAL_X; // Initial X position
    players[player_id].y = INITIAL_Y; // Initial Y position
    player_count++;

    // Inform the client of their ID
    snprintf(buffer, sizeof(buffer), "%d %d %d\n", players[player_id].id, INITIAL_X, INITIAL_Y);
    send(client_sock, buffer, strlen(buffer), 0);

    while (1) {
        ssize_t bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            break; // Client disconnected
        }
        buffer[bytes_received] = '\0'; // Null-terminate the received message
        
        // Broadcast the movement to all other clients
        for (int i = 0; i < player_count; i++) {
            if (i != player_id) { // Do not send to self
                snprintf(buffer, sizeof(buffer), "%d %d %d\n", players[player_id].id, players[player_id].x, players[player_id].y);
                send(players[i].sockfd, buffer, strlen(buffer), 0); // Broadcast to other clients
            }
        }
    }

    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server is running on port %d...\n", PORT);
    
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (client_sock < 0) {
            perror("Accept failed");
            continue; // Continue to accept next connections
        }

        if (player_count < MAX_PLAYERS) {
            handle_client(client_sock); // Handle new client
            printf("Player %d connected.\n", players[player_count - 1].id); // Use id
        } else {
            printf("Maximum players reached, rejecting new client.\n");
            close(client_sock);
        }
    }

    close(server_sock);
    return 0;
}
