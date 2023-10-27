#include <stdio.h>
#include <stdlib.h>
#include "dns.h"

int main(int argc, char **argv)
{
    // validate argc
    if (argc != 2) {
        printf("Expected usage: ./dns <domain name>\n");
        exit(1);
    }

    DNSPacket *packet = send_query("198.41.0.4", argv[1], TYPE_A);
    display_DNSPacket(packet);
    destroy_DNSPacket(&packet);

    /*
    char answer[INET_ADDRSTRLEN] = {0};

    resolve(argv[1], TYPE_A, answer);

    printf("%s -> %s\n", argv[1], answer);
    */

    return 0;
}
