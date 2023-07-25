#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>


int main() {

    /*
    char c, line[100];
    int i = 0;

    while ((c = getchar()) != EOF) {
        if (c == '\n') {
            line[i] = '\0';
            printf("You entered: %s\n", line);
            while (i > 0) { line[i--] = '\0'; }
        } else {
            line[i++] = c;
        }
    }
    */

    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints); // set to 0

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype  = SOCK_DGRAM;

    status = getaddrinfo("www.example.com", "53", &hints, &res);

    printf("DNS query for www.example.com:\n");
    printf("status: %d\n", status);

    for (p = res; p != NULL; p = p->ai_next) {
        printf("\nai_flags: %d\n", p->ai_flags);
        printf("ai_family: %d\n", p->ai_flags);
        printf("ai_socktype: %d\n", p->ai_flags);
        printf("ai_addrlen: %d\n", p->ai_flags);

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            printf("IPv4 address\n");
            printf("sockaddr_in->sin_family: %d\n", ipv4->sin_family);
            printf("sockaddr_in->sin_port: %d / %d\n", ipv4->sin_port, ntohs(ipv4->sin_port));
            printf("sockaddr_in->sin_ipv4: %d\n", ipv4->sin_addr.s_addr);

            inet_ntop(AF_INET, &(ipv4->sin_addr.s_addr), ipstr, INET6_ADDRSTRLEN);
            printf("ip: %s\n", ipstr);

        } else {
            // handle ipv6
            //struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            //addr = &ipv6;
        }

    }

    freeaddrinfo(res);

    return 0;
}
