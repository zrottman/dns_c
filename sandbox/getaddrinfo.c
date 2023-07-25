#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 100

int get_line(char* line, int lim)
{
    int  i;
    char c;

    for (i=0; i<lim-1 && (c=getchar()) != EOF && c != '\n'; i++)
        line[i] = c;
    line[i] = '\0';
    return i;
}

int main() {

    char   line[MAXLINE];
    int    cc;

    struct addrinfo hints, *p; //*res;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints); // set to 0
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype  = SOCK_DGRAM;

    // pack sin_addr from REPL input
    while ((cc = get_line(line, MAXLINE)) > 0) {
        struct addrinfo *res;

        status = getaddrinfo(line, "53", &hints, &res);     // w/ hints
        // status = getaddrinfo(line, "53", NULL, &res);    // w/o hints
        printf("DNS query for %s:\n", line);
        printf("\tstatus: %d\n", status);
        
        for (p = res; p != NULL; p = p->ai_next) {

            if (p->ai_family == AF_INET) {
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
                if (inet_ntop(AF_INET, &(ipv4->sin_addr.s_addr), ipstr, INET6_ADDRSTRLEN) != NULL)
                    printf("\tIPv4 address: %s\n", ipstr);
            } else {
                // handle ipv6
                //struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
                //addr = &ipv6;
            }

        }

        freeaddrinfo(res);

    }
            

    return 0;
}
