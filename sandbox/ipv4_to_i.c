#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main()
{
    char c, chunk[4];
    int i;
    uint32_t addr;

    i = 0;
    addr = 0;

    while ((c = getchar()) != EOF)
    {

        /* convert chunk to int if we reach a `.` or `\n` */
        if (c == '.' || c == '\n') {
            chunk[i] = '\0';
            //printf("Cur chunk %s -> %d\n", chunk, atoi(chunk));

            addr <<= 8;
            addr |= atoi(chunk);
            
            // reset chunk
            for (; i>0; --i) { chunk[i] = '\0'; }

            // output addr and reset on newline
            if (c == '\n') {
                printf("%u\n", addr);
                addr = 0;
            }

        /* else append char to chunk if numeric */
        } else if (i < 3 && c >= '0' && c <= '9'){
            chunk[i++] = c;
        }
    }
    
    return 0;
}

/*
 * to do:
 * - handle cases where atoi(chunk) > 255
 * - handle cases where not expected number of chunks (might be better to read in full ipv4 address for this)
 */
