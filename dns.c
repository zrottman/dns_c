#include <sys/socket.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

uint32_t ipv4_to_int(char *ip)
{
    uint32_t result;
    int      i, j, left, right, res[4];
    char     tmp[4] = "";

    // "1.8.23.0"
    // ['1', '\0', '\0', '\0']
    //
    left = 0;
    right = 0;
    i = 0;
    j = 0;
    result = 0;

    do {
        while (ip[right] != '.')
            right++;
        while (left < right) {
            tmp[i] = ip[left];
            left++;
            i++;
        }
        left = right + 1;
        i = 0;
        res[j] = atoi(tmp);
        
        memset(tmp, '\0', 4);

    } while (ip[right] !='\0');

    return result;



    // res = [1, 8, 23, 0]
    // 00000001 + 00001000 + ? + 000000000


}


uint32_t btoi(char *binstr)
{
    int      i;
    uint32_t result = 0;

    for (i=0; i<32; ++i)
        if (binstr[i] == '1')
            result += 1 << (31 - i);

    return result;

}

int main(int argc, char **argv)
{
    int sockfd;

    // open socket
    //sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    
    // connect to it
    //connect(sockfd, 
    //

    char *ip = "00001000000010000000100000001000";
    uint32_t out;
    out = btoi(ip);
    printf("%d\n",out); // expecting 134744072
                        
    // send stuff
    // get stuff
    // close socket
    // look at stuff

    return 0;
}
