#include <stdio.h>

int main()
{
    char c, ipv4[16];
    int i;

    i = 0;
    while ((c = getchar()) != EOF)
    {
        if (c == '\n') {
            ipv4[i] = '\0';
            printf("You entered: %s\n", ipv4);
            for (; i>0; --i)
                ipv4[i] = '\0';
        } else if (i < 15) {
            ipv4[i++] = c;
        }
    }
}
