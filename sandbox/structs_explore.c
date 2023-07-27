#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

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

    // print struct details
    printf("Printing struct details:\n");
    addrinfo_details();
    sockaddr_details();
    sockaddr_in_details();
    in_addr_details();

    return 0;
}
