#ifndef SERVER_H
#define SERVER_H

#include<sys/socket.h>
// #include<netinet.h>
#include <netinet/in.h>

typedef struct Server {
    int domain;
    int service;
    int protocol;
    int port;
    int backlog;
    u_long interface;
    struct sockaddr_in address;
    void (*launch)(struct Server* server);
    int server_fd;
} Server;

Server server_construction(int domain, int service, int protocol, int port, int backlog, u_long interface, void (*launch)(struct Server* server_old));

// 
// int listen_and_server(char* addr);
// void shut_down_server(int sig);
#endif