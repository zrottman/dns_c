#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAXDATASIZE 4096

int main(int argc, char **argv)
{

    // exit if unexpected argc
    if (argc != 3) {
        printf("usage: webclient <host> <port>\n");
        return EXIT_FAILURE;
    }

    struct addrinfo     *res;
    struct addrinfo     *p;
    int                 sockfd, bytes_sent, bytes_received;
    char                *hostname;
    char                *port;
    char                ipstr[INET6_ADDRSTRLEN];
    char                *request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
    char                buf[MAXDATASIZE];


    port = argv[2];
    hostname = argv[1];

    // use getaddrinfo to load up addrinfo struct
    if (getaddrinfo(hostname, port, NULL, &res) != 0) {
        printf("getaddrinfo error");
        return EXIT_FAILURE;
    }

    // loop through results and try to connect
    for (p = res; p != NULL; p = p->ai_next) {
        // open socket
        if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        // connect 
        if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        perror("failure to connect");
        return 2;
    }
    
        
    
    inet_ntop(p->ai_family, &((struct sockaddr_in *)p->ai_addr)->sin_addr, ipstr, sizeof ipstr);
    printf("client: connected to: %s on port %d\n", ipstr, ntohs(((struct sockaddr_in *)p->ai_addr)->sin_port));
    freeaddrinfo(res);

    // send (or sendall using Beej implementation)
    
    bytes_sent = send(sockfd, request, strlen(request), 0);
    printf("%d of %lu bytes sent\n", bytes_sent, strlen(request));
    
    // recv several times to get all dta
    printf("Response:\n");
    while ((bytes_received = recv(sockfd, buf, MAXDATASIZE-1, 0)) > 0) {
        buf[bytes_received] = '\0';
        printf("%s", buf);
    }

    if (bytes_received < 0) {
        perror("recv");
    }
   

    // close socket
    close(sockfd);


    return 0;
}
