#include <sys/socket.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

uint32_t ipv4_to_int(char *ip)
{
    uint32_t result;
    int i, j, left, right, res[4];
    char tmp[4] = "";

    // "1.8.23.0"
    // ['1', '\0', '\0', '\0']
    //
    left = 0;
    right = 0;
    i = 0;
    j = 0;
    result = 0;

    do
    {
        while (ip[right] != '.')
            right++;
        while (left < right)
        {
            tmp[i] = ip[left];
            left++;
            i++;
        }
        left = right + 1;
        i = 0;
        res[j] = atoi(tmp);

        memset(tmp, '\0', 4);

    } while (ip[right] != '\0');

    return result;

    // res = [1, 8, 23, 0]
    // 00000001 + 00001000 + ? + 000000000
}

uint32_t btoi(char *binstr)
{
    int i;
    uint32_t result = 0;

    for (i = 0; i < 32; ++i)
        if (binstr[i] == '1')
            result += 1 << (31 - i);

    return result;
}


int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 

int main(int argc, char **argv)
{
    int sockfd;
    socklen_t len;
    char buf[512];
    struct sockaddr_in dest;
    struct sockaddr_storage store;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = 134744072;
    dest.sin_family = AF_INET;
    // open socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    char *query = "D\xcb\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x07example\x03com\x00\x00\x01\x00\x01";
    int st, rf;

    st = sendto(sockfd, query, strlen(query) + 1, 0, (struct sockaddr *)&dest, sizeof dest);
    if (st == -1)
    {
        perror("sendto");
    }
    // rf = recvfrom(sockfd, buf, sizeof buf, 0, (struct sockaddr *)&store, &len);
    // if (rf == -1)
    // {
    //     perror("recvfrom");
    // }
    printf("num of bytes sent: %d\n", st);
    // printf("from recvfrom %d\n", rf);
    printf("buf dude: %s\n", buf);
    close(sockfd);
    // connect to it
    // connect(sockfd,
    //

    // char *ip = "00001000000010000000100000001000";
    // uint32_t out;
    // out = btoi(ip);
    // printf("%d\n",out); // expecting 134744072

    // send stuff
    // get stuff
    // close socket
    // look at stuff

    return 0;
}
