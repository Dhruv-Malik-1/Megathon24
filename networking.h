#ifndef NETWORKING_H
#define NETWORKING_H

#include <stddef.h>   // For size_t
#include <sys/types.h> // For ssize_t
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h> // For inet_addr
#include <unistd.h> // For close
#include <stdio.h> // For printf
#include <stdlib.h> // For exit

int create_socket();
int bind_socket(int sockfd, int port);
int listen_socket(int sockfd);
int accept_connection(int sockfd);
ssize_t send_data(int sockfd, const void *buffer, size_t length);
ssize_t receive_data(int sockfd, void *buffer, size_t length);
void close_socket(int sockfd);

#endif // NETWORKING_H
