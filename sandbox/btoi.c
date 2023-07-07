#include <stdio.h>
#include <stdint.h>

/* convert bitstring[32] to 32-bit uint */
uint32_t btoi(char *binstr, int len)
{
    int      i;
    uint32_t result;

    result = 0;
    for (i=0; i<len; ++i)
        if (binstr[i] == '1')
            result += 1 << (len - 1 - i);

    return result;
}

int main()
{
    char c, binstr[32];
    int i = 0;

    while ((c = getchar()) != EOF)
    {
        if (c != '\n')
        {
            if (i < 32)
                // add `c` to `binstr` and increment `i` while `i` < 8
                binstr[i++] = c;
        } else {
            // print result
            printf("-> %u\n", btoi(binstr, i));
            // reset all `binstr` elements to null
            for (; i>0; --i)
                binstr[i] = '\0';
        }
    }

    return 0;
}
