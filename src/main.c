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

    char answer[INET_ADDRSTRLEN] = {0};  // buffer for answer string

    resolve(argv[1], TYPE_A, answer);

    printf("%s\n", answer);

    return 0;
}
