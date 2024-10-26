#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 10

int client_count = 0;
int client_sockets[MAX_CLIENTS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast_message(const char *message) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            send(client_sockets[i], message, strlen(message), 0);
        }
    }
    pthread_mutex_unlock(&mutex);
}

void broadcast_new_client(int new_socket, int client_number) {
    char message[50];
    snprintf(message, sizeof(message), "New client %d connected!\n", client_number);
    broadcast_message(message);
}

void *handle_client(void *client_socket) {
    int sock = *(int *)client_socket;
    free(client_socket);
    char buffer[1024] = {0};
    
    pthread_mutex_lock(&mutex);
    int client_number = ++client_count;
    client_sockets[client_number - 1] = sock; // Store client socket
    pthread_mutex_unlock(&mutex);

    // Send welcome message to client
    char *welcome_message = "Welcome to the Halloween game server!";
    send(sock, welcome_message, strlen(welcome_message), 0);

    // Broadcast new client connection
    broadcast_new_client(sock, client_number);

    while (1) {
        // Receive message from the client
        int valread = read(sock, buffer, 1024);
        if (valread <= 0) {
            break; // Break the loop if the client disconnected
        }

        // Print and echo the message back to the client
        printf("Client %d says: %s\n", client_number, buffer);
        send(sock, buffer, valread, 0); // Echo the message back
    }

    printf("Client %d disconnected\n", client_number);

    // Lock mutex to remove client and decrease count
    pthread_mutex_lock(&mutex);
    client_sockets[client_number - 1] = 0; // Remove client socket
    client_count--;
    pthread_mutex_unlock(&mutex);

    // Close the connection
    close(sock);
    return NULL;
}

void handle_sigint(int sig) {
    printf("Server shutting down...\n");
    broadcast_message("Server is shutting down. All clients will be disconnected.\n");
    sleep(1);  // Give time for the message to be sent
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] != 0) {
            close(client_sockets[i]);
        }
    }
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);  // Handle Ctrl+C signal

    int server_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket option to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Setting up the server address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    address.sin_port = htons(PORT);

    // Binding the socket to the network address and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d\n", PORT);

    while (1) {
        // Accept incoming client connection
        int *new_socket = malloc(sizeof(int));
        if ((*new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            free(new_socket);
            continue;
        }
        printf("New client connected\n");

        // Create a new thread to handle the client
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, (void *)new_socket) != 0) {
            perror("Could not create thread");
            free(new_socket);
            continue;
        }
        pthread_detach(thread_id);  // Detach the thread to handle cleanup automatically
    }

    close(server_fd);
    return 0;
}
