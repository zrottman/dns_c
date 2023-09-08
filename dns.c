#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "request.h"

int main(int argc, char **argv)
{

    /* send request */
    int                     sockfd;
    struct sockaddr_in      dest;
    char                    buf[512];
    struct sockaddr_storage from;
    socklen_t               fromlen;

    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = 0x08080808; // 8.8.8.8
    dest.sin_family = AF_INET;


    // validate argc
    if (argc != 2) {
        printf("Expected usage: ./dns <domain name>\n");
        exit(1);
    }

    // open socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    // create query
    DNSQuery query = NewDNSQuery(argv[1], TYPE_A);
    
    // send query
    ssize_t bytes_sent = sendto(sockfd, query.s, query.len, 0, (struct sockaddr *)&dest, sizeof dest);
    if (bytes_sent == -1)
        perror("sendto");
    printf("bytes sent: %ld\n", bytes_sent);

    // receive result
    ssize_t bytes_received = recvfrom(sockfd, buf, sizeof buf, 0, (struct sockaddr *)&from, &fromlen);
    if (bytes_received == -1)
        perror("recvfrom");

    // print result
    printf("Bytes received: %zd\n", bytes_received);
    printf("Response: ");
    for (int i=0; i<bytes_received; ++i)
        printf("%c", buf[i]);
    printf("\n");

    // close socket
    close(sockfd);

    // parse response
    DNSHeader   *header   = calloc(1, sizeof(DNSHeader));
    DNSQuestion *question = calloc(1, sizeof(DNSQuestion));
    DNSRecord   *record   = calloc(1, sizeof(DNSRecord));
    int          bytes_read;

    bytes_read = parse_header(buf, header);                 // parse header
    bytes_read = parse_question(question, buf, bytes_read); // parse question
    bytes_read = parse_record(record, buf, bytes_read);     // parse record
    
    // display structs
    display_DNSHeader(header);
    display_DNSQuestion(question);
    display_DNSRecord(record);

    return 0;
}
