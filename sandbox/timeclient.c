/*
 *
 * Adapted from Beej's Guide to Network Concepts
 * https://beej.us/guide/bgnet0/
 *
 */


#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

uint32_t bytes_to_int(char *b, int len) {
    uint32_t seconds = 0;
    for (int i=0; i<len; i++) { seconds = (seconds << 8) | (b[i] & 0Xff); }
    return seconds;
}

uint32_t system_seconds_since_1900(void) {
    uint32_t s_delta, seconds_since_1900_epoch;
    s_delta = 2208988800;
    seconds_since_1900_epoch = s_delta + time(NULL);
    return seconds_since_1900_epoch;
}

int main()
{
    struct addrinfo hints, *res, *p;
    int sockfd, bytes_received;
    char buf[5];
    uint32_t seconds;

    // configure hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // resolve IP address with getaddrinfo
    if (getaddrinfo("time.nist.gov", "37", &hints, &res) != 0) {
        perror("error: getaddrinfo");
        return EXIT_FAILURE;
    }

    // connect to time.nist.gov
    for (p = res; p != NULL; p = p->ai_next) {
        // open socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        // connect
        if ((connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
            close(sockfd);
            perror("connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        perror("failure to connect");
        return EXIT_FAILURE;
    }

    freeaddrinfo(res);

    // receive 4 bytes
    if ((bytes_received = recv(sockfd, buf, 4, 0)) != 4) {
        perror("did not received expected number of bytes");
        close(sockfd);
        return EXIT_FAILURE;
    }

    buf[bytes_received] = '\0';
    seconds = bytes_to_int(buf, bytes_received);

    // print data received
    printf("Seconds since UNIX Epich:\n");
    printf("NIST  : %u\n", seconds);
    printf("System: %u\n", system_seconds_since_1900());

    // close
    close(sockfd);


    return 0;
}
