#include"server.h"
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<signal.h>

// #define BACKLOG 10

// volatile sig_atomic_t shutdown_signal;

// int listen_and_server(char* addr) {
//     server_fd = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_fd < 0) {
//         return -1;
//     }

//     // struct sockaddr_in addr;

//     shutdown_signal = 0;
//     return 1;
// }

Server server_construction(int domain, int service, int protocol, int port, int backlog, u_long interface, void (*launch)(struct Server* server_old)) {
    Server server;

    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.port = port;
    server.backlog = backlog;
    server.interface = interface;

    server.address.sin_family = server.domain;
    server.address.sin_port = htons(server.port);
    server.address.sin_addr.s_addr = htonl(server.interface);

    server.server_fd = socket(server.domain, server.service, server.protocol);
    if (server.server_fd < 0) {
        // error
        exit(1);
    }

    if (bind(server.server_fd, (struct sockaddr *) &server.address, sizeof(server.address)) < 0) {
        exit(1);
    }

    if (listen(server.server_fd, server.backlog) < 0) {
        exit(1);
    }

    server.launch = launch;

    return server;
}