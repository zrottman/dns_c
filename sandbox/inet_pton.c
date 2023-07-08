#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main()
{
    uint32_t s_addr; // 4-bytes to hold address
    char buf[16];    // Buffer to hold max IPv4 addr plus null string terminator
    char c;
    int i = 0;

    while ((c = getchar()) != EOF)
    {
        if (c == '\n') {
            buf[i] = '\0';
            printf("Buf: %s\n", buf);
            if (inet_pton(AF_INET, buf, &s_addr) == 1) {
                printf("--> Host Byte Order:    %u\n", s_addr);
                printf("--> Network Byte Order: %u\n\n", htonl(s_addr));
            }
            s_addr = 0;
            for (; i>0; --i)
                buf[i] = '\0';
        } else {
            if (i < 15)
                buf[i++] = c;
        }
    }

    return 0;
}
