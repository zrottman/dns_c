#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{

    // exit if unexpected argc
    if (argc != 3) {
        printf("usage: webclient <host> <port>\n");
        return EXIT_FAILURE;
    }

    struct addrinfo *res, *p;
    struct sockaddr_in *addr;
    int sockfd;
    char *hostname, *port, ipstr[INET6_ADDRSTRLEN];

    port = argv[2];
    hostname = argv[1];

    // getaddrinfo()
    if (getaddrinfo(hostname, port, NULL, &res) != 0) {
        printf("getaddrinfo error");
        return EXIT_FAILURE;
    }

    printf("RESULTS\n");

    for (p = res; p != NULL; p = p->ai_next) {
        addr = (struct sockaddr_in *)p->ai_addr;
        inet_ntop(p->ai_family, &(addr->sin_addr), ipstr, sizeof ipstr);
        printf("IP: %u -> %s\n", ntohl(addr->sin_addr.s_addr), ipstr);
    }

    


    // open socket
    //sockfd socket();
    
    // connect to socket with host and port

    // send (or sendall using Beej implementation)
    
    // recv several times to get all dta


    return 0;
}
