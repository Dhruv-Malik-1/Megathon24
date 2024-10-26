#include "networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Create a socket
int create_socket() {
    return socket(AF_INET, SOCK_STREAM, 0);
}

// Bind the socket to a port
int bind_socket(int sockfd, int port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    return bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
}

// Start listening for incoming connections
int listen_socket(int sockfd) {
    return listen(sockfd, 5); // Listen for up to 5 connections
}

// Accept a new connection
int accept_connection(int sockfd) {
    return accept(sockfd, NULL, NULL);
}

// Send data to a socket
ssize_t send_data(int sockfd, const void *buffer, size_t length) {
    return send(sockfd, buffer, length, 0);
}

// Receive data from a socket
ssize_t receive_data(int sockfd, void *buffer, size_t length) {
    return recv(sockfd, buffer, length, 0);
}

// Close a socket
void close_socket(int sockfd) {
    close(sockfd);
}
