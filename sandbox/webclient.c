#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define MAXDATASIZE 1000 
#define MAXREQUESTSIZE 200

// Function:    build_request
// Description: Builds HTTP request header using hostname supplied as arg.
// Arguments:   
//              - host: hostname
//              - buf:  output buffer
// Returns:     void
void build_request(char *host, char *buf)
{
    strcpy(buf, "GET / HTTP/1.1\r\nHOST: \0");
    strcat(buf, host);
    strcat(buf, "\r\nConnection: close\r\n\r\n");
}

int main(int argc, char **argv)
{

    // exit if unexpected argc
    if (argc != 3) {
        printf("usage: webclient <host> <port>\n");
        return EXIT_FAILURE;
    }

    struct addrinfo     *res;
    struct addrinfo     *p;
    struct addrinfo     hints;
    int                 sockfd, bytes_sent, bytes_received;
    char                *hostname;
    char                *port;
    char                ipstr[INET6_ADDRSTRLEN];
    char                request[MAXREQUESTSIZE];
    char                buf[MAXDATASIZE];

    port = argv[2];
    hostname = argv[1];

    // configure hints
    memset(&hints, 0, sizeof hints); // initialize to 0
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // build request
    build_request(hostname, request);

    // use getaddrinfo to load up addrinfo struct
    if (getaddrinfo(hostname, port, &hints, &res) != 0) {
        printf("getaddrinfo error");
        return EXIT_FAILURE;
    }

    // loop through results and try to connect
    for (p = res; p != NULL; p = p->ai_next) {
        // open socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        // connect 
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
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
    printf("Connected to %s on port %d\n\n", ipstr, ntohs(((struct sockaddr_in *)p->ai_addr)->sin_port));
    freeaddrinfo(res);

    // send (or sendall using Beej implementation)
    bytes_sent = send(sockfd, request, strlen(request), 0);
    printf("Sending request:\n%s", request);
    printf("%d of %lu bytes sent\n\n", bytes_sent, strlen(request));
    
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
