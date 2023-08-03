/* Program:     getaddrinfo 
 * Description: A simple program that takes human-readable domains (e.g.,
 *              www.example.com) and uses getaddrinfo() to resolve the
 *              IP address(es). Based on example from Beej's guide.
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 100

// Function:    get_line
// Description: Reads in a line character-by-character to `line`.
// Arguments:   
//              line:   buffer to hold characters of line
//              lim:    max lengt of line
// Returns:     i:      int, character count of line
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

    struct addrinfo hints, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    // build hints struct
    memset(&hints, 0, sizeof hints); // initialize hints struct to 0
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype  = SOCK_DGRAM;
    // hints.ai_socktype  = SOCK_STREAM;

    // pack sin_addr from REPL input
    while ((cc = get_line(line, MAXLINE)) > 0) {
        struct addrinfo *res, *addr;

        status = getaddrinfo(line, "53", &hints, &res);     // version w/ hints
        // status = getaddrinfo(line, "53", NULL, &res);    // version w/o hints
        printf("DNS query for %s:\n", line);
        printf("\tstatus: %d\n", status);
        
        // traverse results linked list
        for (p = res; p != NULL; p = p->ai_next) {

            void *addr;
            char *ipversion;

            // parse node based on whether result is IPv4 or IPv6
            if (p->ai_family == AF_INET) { // IPv4
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
                addr = &(ipv4->sin_addr.s_addr);
                ipversion = "IPv4";
            } else { // IPv6
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
                addr = &(ipv6->sin6_addr.s6_addr);
                ipversion = "IPv6";
            }

            // print IP
            if (inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr) != NULL)
                printf("\t%s: %s\n", ipversion, ipstr);

        }

        freeaddrinfo(res);

    }

    return 0;
}
