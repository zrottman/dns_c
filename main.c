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

    DNSPacket *packet = send_query("8.8.8.8", argv[1], TYPE_A);
    display_DNSPacket(packet);

    return 0;
}
