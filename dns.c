#include <sys/socket.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <arpa/inet.h>
#include "request.h"

int main(int argc, char **argv)
{
    int                     sockfd;
    socklen_t               len;
    char                    buf[512];
    struct sockaddr_in      dest;
    struct sockaddr_storage store;

    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = 0x08080808; // 8.8.8.8
    dest.sin_family = AF_INET;

    // open socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    // create Query
    char* d_n = "www.example.org";
    Query full_query = NewDNSQuery(d_n, 0);
    
    // send query
    ssize_t st = sendto(sockfd, full_query.s, full_query.len, 0, (struct sockaddr *)&dest, sizeof dest);
    if (st == -1)
    {
        perror("sendto");
    }
    printf("bytes sent: %ld\n", st);

    // receive result
    ssize_t rf = recvfrom(sockfd, buf, sizeof buf, 0, (struct sockaddr *)&store, &len);
    if (rf == -1)
    {
        perror("recvfrom");
    }

    // print result
    printf("bytes received: %zd\n", rf);
    printf("response: ", buf);
    for (int i=0; i<rf; ++i)
        printf("%c", buf[i]);
    printf("\n");

    // close socket
    close(sockfd);

    // TODO: PRINT STRUCT

    return 0;
}
