#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main()
{
    char c, chunk[4];
    int i, chunk_as_i, chunks_count;
    uint32_t addr;

    i = 0;
    addr = 0;
    chunks_count = 0;

    while ((c = getchar()) != EOF)
    {

        /* convert chunk to int if we reach a `.` or `\n` */
        if (c == '.' || c == '\n') {
            chunk[i] = '\0';            // append null to chunk
            addr <<= 8;                 // shift addr 8 bits left
            chunk_as_i = atoi(chunk);   // convert current chunk to int
            if (chunk_as_i > 255) {     // exit on invalid chunk
                printf("Invalid IPv4 address component: %s\n", chunk);
                return 1;
            }
            addr |= atoi(chunk);        // add chunk to addr
            
            // reset chunk
            for (; i>0; --i) { chunk[i] = '\0'; }

            // inc chunks_count
            chunks_count++;

            // output addr and reset on newline
            if (c == '\n') {
                if (chunks_count == 4) {
                    printf("%u\n", addr);
                } else {
                    printf("Unexpected IPv4 address format ");
                    for (int j=1; j<chunks_count; ++j)
                        printf("x.");
                    printf("x\n");
                    return 1;
                }
                chunks_count = 0;
                addr = 0;
            }

        /* else append char to chunk if numeric */
        } else if (i < 3 && c >= '0' && c <= '9'){
            chunk[i++] = c;
        }
    }
    
    return 0;
}
