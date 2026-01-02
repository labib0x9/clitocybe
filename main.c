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
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>

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
    struct sockaddr_storage s_addr;
    addr_t addr;
    int err;
    int fd;
} listener_t;

const int BACKLOG = 10;

enum {
    PROTOCOL_TCP,
    PROTOCOL_UDP,
};

enum {
    SCREATE_ERR,

};

int getProtocolType(const char* protocol) {
    if (strcasecmp(protocol, "tcp") == 0) return PROTOCOL_TCP;
    if (strcasecmp(protocol, "udp") == 0) return PROTOCOL_UDP;
    return -1;
}

// Seperate IPv4, return 0 on success
// "127.0.0.1:8080"
// ":8080"
// "255.255.255.255:65536"
// no invalid address or port handling right now.
int splitAddrPort(addr_t* addr, const char* address) {
    if (!addr) return -1;
    if (!address) return -1;

    // seperate by ':'
    char* colonAt = strchr(address, ':');
    if (!colonAt) return -1;

    // address
    strncpy(addr->host, address, colonAt - address);
    addr->host[colonAt - address] = '\0';

    // port
    addr->port = atoi(colonAt + 1);
    return 0;
}

void tcpCreateAndListenIpv4(listener_t* ln) {
    // create socket
    ln->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ln->fd < 0) {
        ln->err = 2;
        return;
    }

    // configure socket
    int opt = 1;
    if (setsockopt(ln->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsocket error");
        ln->err = 3;
        return;
    }
    // setsockopt(ln->fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    // setsockopt(ln->fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));


    // setup listening address
    // memset(&(ln->s_addr), 0, sizeof(ln->s_addr));
    // ln->s_addr.ss_family = AF_INET;
    // ln->s_addr.
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(ln->addr.port);
    if (ln->addr.host[0] == '\0') {
        addr.sin_addr.s_addr = INADDR_ANY;
        printf("Any address\n");
    } else {
        if (inet_pton(AF_INET, ln->addr.host, &addr.sin_addr) == -1) {
            perror("inet_pton error");
            ln->err = 4;
            return;
        }
    }
    memcpy(&(ln->s_addr), &addr, sizeof(addr));

    // bind
    if (bind(ln->fd, (struct sockaddr*) &(ln->s_addr), sizeof(ln->s_addr))== -1) {
        perror("bind error");
        ln->err = 5;
        return;
    }

    // listen
    if (listen(ln->fd, BACKLOG)== -1) {
        perror("listen error");
        ln->err = 6;
        return;
    }
}

// We will use only IPv4 and TCP only.
// network is TCP.
// err = 0, then no error
// err = 1, unknown protocol
// err = 2, socket creation
listener_t SListen(const char* network, const char* address) {
    listener_t ln;
    ln.err = 0;
    ln.fd = 0;
    ln.err = splitAddrPort(&(ln.addr), address);
    if (ln.err != 0) return ln;

    switch (getProtocolType(network)) {
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

void SClose(listener_t ln) {
    close(ln.fd);
}

// SAccept(ln) -> return one client
// err ->
// 0 - > passed
// 1 -> accept error
typedef struct {
    int fd;
    struct sockaddr_storage addr;
    int err;
} client_t;

// listen on listener and accept incoming connection, store the address and return one client.
client_t SAccept(listener_t ln) {
    client_t conn;
    conn.err = 0;
    socklen_t len = sizeof(conn.addr);
    printf("Listening...\n");
    conn.fd = accept(ln.fd, (struct sockaddr*) &conn.addr, &len);
    if (conn.fd < 0) {
        conn.err = 1;
        return conn;
    }
    printf("Accepted... %d\n", conn.fd);

    return conn;
}

void ConnClose(client_t client) {
    close(client.fd);
}

int main() {

    listener_t ln = SListen("tcp", ":8080");
    if (ln.err != 0) {
        perror("listerner failed");
        // exit(1);
        return 0;
    }

    // if (strlen(ln.addr.host) == 0) {
    //     printf("NULL\n");
    // }

    printf("%s  %d\n", ln.addr.host, ln.addr.port);
    printf("socket fd = %d\n", ln.fd);

    client_t conn = SAccept(ln);



    ConnClose(conn);
    SClose(ln);

    // Test getProtocolType() function.
    // printf("%d\n", getProtocolType("tcp"));
    // printf("%d\n", getProtocolType("udp"));

    // printf("%d\n", getProtocolType("tCp"));
    // printf("%d\n", getProtocolType("uDp"));

    return 0;
}