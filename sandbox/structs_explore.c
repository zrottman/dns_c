#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

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

// print size details for addrinfo
void addrinfo_details(void)
{
    struct addrinfo test;
    printf("addrinfo size:               %2lu @ %p\n", sizeof test, &test);
    printf("int       ai_flags size:     %2lu @ %p\n", sizeof test.ai_flags, &(test.ai_flags));
    printf("int       ai_family size:    %2lu @ %p\n", sizeof test.ai_family, &(test.ai_family));
    printf("int       ai_socktype size:  %2lu @ %p\n", sizeof test.ai_socktype, &(test.ai_socktype));
    printf("int       ai_protocol size:  %2lu @ %p\n", sizeof test.ai_protocol, &(test.ai_protocol));
    printf("socklen_t ai_addrlen size:   %2lu @ %p\n", sizeof test.ai_addrlen, &(test.ai_addrlen));
    printf("char*     ai_canonname size: %2lu @ %p\n", sizeof test.ai_canonname, &(test.ai_canonname));
    printf("sockaddr* ai_addr size:      %2lu @ %p\n", sizeof test.ai_addr, &(test.ai_addr));
    printf("addrinfo* ai_next size:      %2lu @ %p\n", sizeof test.ai_next, &(test.ai_next));
    printf("\n");
}

// print size details for sockaddr
void sockaddr_details(void)
{
    struct sockaddr test;
    printf("sockaddr size:               %2lu @ %p\n", sizeof test, &test);
    printf("short     sa_family size:    %2lu @ %p\n", sizeof test.sa_family, &(test.sa_family));
    printf("char      sa_data[14] size:  %2lu @ %p\n", sizeof test.sa_data, &(test.sa_data));
    printf("\n");
}

// print size details for sockaddr_in
void sockaddr_in_details(void)
{
    struct sockaddr_in test;
    printf("sockadddr_in size:           %2lu @ %p\n", sizeof test, &test);
    printf("short     sin_family size:   %2lu @ %p\n", sizeof test.sin_family, &(test.sin_family));
    printf("u_short   sin_port size:     %2lu @ %p\n", sizeof test.sin_port, &(test.sin_port));
    printf("in_addr   sin_addr size:     %2lu @ %p\n", sizeof test.sin_addr, &(test.sin_addr));
    printf("char      sin_zero size:     %2lu @ %p\n", sizeof test.sin_zero, &(test.sin_zero));
    printf("\n");
}

// print size details for in_addr
void in_addr_details(void)
{
    struct in_addr test;
    printf("in_addr size:                %2lu @ %p\n", sizeof test, &test);
    printf("u_long    s_addr size:       %2lu @ %p\n", sizeof test.s_addr, &(test.s_addr));
    printf("\n");
}

int main() {

    char   line[MAXLINE];
    int    cc, sockfd;
    struct sockaddr_in addr, resolver;

    // print struct details
    printf("Printing struct details:\n");
    addrinfo_details();
    sockaddr_details();
    sockaddr_in_details();
    in_addr_details();



    // pack resolver struct
    resolver.sin_family = AF_INET;
    resolver.sin_port   = htons(53);
    inet_pton(AF_INET, "8.8.8.8", &(resolver.sin_addr));

    // 1. build query
    // 2. open socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    // 3. connect?
    if (connect(sockfd, (struct sockaddr *)&resolver, sizeof resolver) == 0) {
        // do something
        printf("Successful connection!\n");
    }

    // 4. send to?

    // 5. deal with response
    
    // 6. close 
    close(sockfd);




    // pack default parts of sockaddr_in
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(53);

    while ((cc = get_line(line, MAXLINE)) > 0) {
        if (inet_pton(AF_INET, line, &(addr.sin_addr)) != 1) { // invalid IP address format
            printf("Invalid IP address format: %s\n", line);
            exit(1);
        }

        printf("%s --> %d\n", line, ntohl(addr.sin_addr.s_addr));
    }

    return 0;
}
