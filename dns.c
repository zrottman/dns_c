#include <sys/socket.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

typedef struct dns_header
{
    u_int16_t id;
    u_int16_t flags;
    u_int16_t num_questions;
    u_int16_t num_answers;
    u_int16_t num_authorities;
    u_int16_t num_additionals;
} dns_header;

typedef struct dns_question
{
    char *name; // cast?
    u_int16_t type;   // set to 1 for some reason
    u_int16_t class;  // set to 1 for interent
} dns_question;

dns_header *new_header(u_int16_t id, u_int16_t flags, u_int16_t num_questions)
{
    dns_header *header = calloc(1, sizeof(dns_header));
    header->id = id;
    header->flags = flags;
    header->num_questions = num_questions;
    return header;
};

dns_question *new_question(char *name)
{
    dns_question *question = calloc(1, sizeof(dns_question));
    question->name = malloc(strlen(name) + 1);
    strcpy(question->name, name);
    question->class = 1;
    question->type = 1;
    return question;
}

// def encode_dns_name(domain_name):
//     encoded = b""
//     for part in domain_name.encode("ascii").split(b"."):
//         encoded += bytes([len(part)]) + part
//     return encoded + b"\x00"

int encode_dns_name(char *domain_name, char *res)
{
    char *token;
    int name_len = strlen(domain_name);
    int j = 0;
    while ((token = strsep(&domain_name, ".")))
    {
        res[j++] = strlen(token);
        strcpy(res + j, token);
        j += strlen(token);
    }
    res[j] = '\0';
    return j + 1;
}

// char* build_query()
// {
// }

uint32_t ipv4_to_int(char *ip)
{
    uint32_t result;
    u_int16_t i, j, left, right, res[4];
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

int sendall(int s, char *buf, int *len, struct sockaddr_in dest)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while (total < *len)
    {
        n = sendto(s, buf + total, bytesleft, 0, (struct sockaddr *)&dest, sizeof(dest));
        if (n == -1)
        {
            break;
        }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

int main(int argc, char **argv)
{
    int sockfd;
    socklen_t len;
    char buf[512];
    struct sockaddr_in dest;
    struct sockaddr_storage store;
    dest.sin_port = htons(53);
    // dest.sin_addr.s_addr = 0x08080808; // 8.8.8.8
    dest.sin_addr.s_addr = htonl(0x08080404); // 8.8.4.4
    dest.sin_family = AF_INET;
    // open socket
    // sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    // char query[] = "D" "\xcb\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x07" "example" "\x03" "com" "\x00\x00\x01\x00\x01";
    char query[] = "\x44\xcb\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x07\x65\x78\x61\x6d\x70\x6c\x65\x03\x63\x6f\x6d\x00\x00\x01\x00\x01";

    // for (int i = 0; i < sizeof(query); i++) {
    //     printf("%hhx %hhu %c \n", query[i], query[i], query[i]);
    // }

    int st, rf;

    // maybe fixes our problem?
    int query_len = sizeof(query) / sizeof(char);
    int sendall_len = query_len;
    // if (sendall(sockfd, query, &sendall_len, dest) == -1)
    // {
    //     printf("bad sendall");
    //     return 1;
    // }
    // printf("num of bytes sent: %d\n", sendall_len);
    // assert(sendall_len == query_len);

    char d_n[] = "www.example.org";
    char encoded_d_n[100];
    int res_len = encode_dns_name(d_n, encoded_d_n);
    printf("res len %d\n", res_len);
    // st = sendto(sockfd, query, strlen(query) + 1, 0, (struct sockaddr *)&dest, sizeof dest);
    // if (st == -1)
    // {
    //     perror("sendto");
    // }
    // printf("num of bytes sent: %d\n", st);

    // rf = recvfrom(sockfd, buf, sizeof buf, 0, (struct sockaddr *)&store, &len);
    // if (rf == -1)
    // {
    //     perror("recvfrom");
    // }
    // printf("from recvfrom %d\n", rf);
    // printf("buf dude: %s\n", buf);
    // close(sockfd);
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
