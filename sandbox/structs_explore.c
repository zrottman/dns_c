#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAXLINE 100

// Function:    get_line
// Description: Reads in a line character-by-character to `line`.
// Arguments:   
//              line:   buffer to hold characters of line
//              lim:    max lengt of line
// Returns:     i:      int, character count of line
int get_line(char* line, int lim)
{
    int  i;
    char c;

    for (i=0; i<lim-1 && (c=getchar()) != EOF && c != '\n'; i++)
        line[i] = c;
    line[i] = '\0';
    return i;
}

int main() {

    char   line[MAXLINE];
    int    cc;
    struct sockaddr_in addr;

    // pack default parts of sockaddr_in
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(53);

    while ((cc = get_line(line, MAXLINE)) > 0) {
        if (inet_pton(AF_INET, line, &(addr.sin_addr)) != 1) { // invalid IP address format
            printf("Invalid IP address format: %s\n", line);
            exit(1);
        }

        printf("%s --> %d\n", line, ntohl(addr.sin_addr.s_addr));
    }

    return 0;
}
