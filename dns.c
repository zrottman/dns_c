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
    int                     sockfd;
    struct sockaddr_in      dest;
    char                    buf[512];
    struct sockaddr_storage from;
    socklen_t               fromlen;

    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = 0x08080808; // 8.8.8.8
    dest.sin_family = AF_INET;

    // open socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    // create query
    Query query = NewDNSQuery("www.example.org", TYPE_A);
    
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
    char decoded_name[100];

    DNSHeader *responseheader = calloc(1, sizeof(DNSHeader));
    int bytes_read = parse_header(buf, responseheader);
    display_DNSHeader(responseheader);
    
    DNSQuestion *responsequestion = calloc(1, sizeof(DNSQuestion));
    bytes_read = parse_question(responsequestion, buf, bytes_read);
    display_DNSQuestion(responsequestion);

    int pointer = decode_compressed_name(buf, bytes_read);
    printf("decoded pointer: %hu\n", pointer);

    //DNSRecord *responserecord = calloc(1, sizeof(DNSRecord));
    //bytes_read = parse_record(buf, responserecord, bytes_read);

    //bytes_read = decode_name(buf, decoded_name, bytes_read);
    return 0;
}
