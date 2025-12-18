// #include<stdio.h>
// #include<string.h>
// #include "server.h"

// void registration() {
//     // Register_Route("/health", "GET", health_handler);
// }

// void launch(Server* server) {
//     printf("Waiting\n");
//     int new_socket = accept(server->server_fd, (struct sockaddr *) &(server->address), sizeof(server->address));
//     char buffer[3000];
//     int n = read(new_socket, buffer, sizeof(buffer) - 1);
//     buffer[n] = '\0';
//     printf("%s\n", buffer);


//     const char *HTTP_200_OK =
//     "HTTP/1.1 200 OK\r\n"
//     "Content-Type: text/html\r\n"
//     "Content-Length: %d\r\n"
//     "Connection: Closed\r\n"
//     "\r\n"
//     "<html><body><h1>HELLO FROM SERVER = WORLD</h1></body></body>";

//     write(new_socket, HTTP_200_OK, strlen(HTTP_200_OK));
//     close(new_socket);

//     printf("Done\n");
// }

// int main() {

//     // registration();

//     // int err = listen_and_server("127.0.0.1:8080");
//     // if (err == -1) {
//     //     return 1;
//     // }

//     Server server = server_construction(AF_INET, SOCK_STREAM, 0, 8080, 10, INADDR_ANY, launch);
//     server.launch(&server);

//     return 0;
// }


#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>

/**  Need To Implement -> net.Listen("tcp", ":8080")  **/
/*
    SListen("tcp", ":8080")
    -> supports ipv4 and ipv6 in single implementation. (Only ipv4 for now)
    -> Return a ready-to-accept fd, abstract socket creation, options set, bound, listening.
    -> Expose zero protocol trivia to caller. no sockaddr_in, AF_INET.
    -> Return explicit error. Caller can inspect caused error.
    -> Returns listener_t
*/

typedef struct {
    int port;
    char host[24];
} addr_t;

typedef struct {
    addr_t addr;
    int err;
    int fd;
} listener_t;

enum {
    PROTOCOL_TCP,
    PROTOCOL_UDP,
};

int getProtocolType(const char* protocol) {
    if (strcmp(protocol, "tcp") || strcmp(protocol, "TCP")) return PROTOCOL_TCP;
    if (strcmp(protocol, "udp") || strcmp(protocol, "UDP")) return PROTOCOL_UDP;
    return -1;
}

// Seperate IPv4, return 0 on success
// "127.0.0.1:8080"
// ":8080"
// "255.255.255.255:65536"
int splitAddrPort(addr_t* addr, const char* address) {
    if (!addr) return -1;
    if (!address) return -1;
    char* colonAt = strchr(address, ':');

    // address
    strncpy(addr->host, address, colonAt - address);
    addr->host[colonAt - address] = '\0';

    // port
    addr->port = atoi(colonAt + 1);
    return 0;
}

void tcpCreateAndListenIpv4(listener_t* ln) {
    ln->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ln->fd < 0) {
        ln->err = 2;
        return;
    }
}

// We will use only IPv4 and TCP only.
// err = 0, then no error
// err = 1, unknown protocol
// err = 2, socket creation
listener_t SListen(const char* protocol, const char* address) {
    listener_t ln;
    ln.err = splitAddrPort(&(ln.addr), address);
    if (ln.err != 0) return ln;

    switch (getProtocolType(protocol)) {
    case PROTOCOL_TCP:
        tcpCreateAndListenIpv4(&ln);
        break;
    case PROTOCOL_UDP:
        break;
    default: {
            ln.err = 1;
            // return ln;
        }
    }

    return ln;
}

void SClose(listener_t* ln) {
    close(ln->fd);
}

int main() {

    listener_t ln = SListen("tcp", ":8080");
    if (ln.err != 0) {
        perror("listerner failed");
        // exit(1);
        return 0;
    }

    if (strlen(ln.addr.host) == 0) {
        printf("NULL\n");
    }

    printf("%s  %d\n", ln.addr.host, ln.addr.port);

    SClose(&ln);

    return 0;
}