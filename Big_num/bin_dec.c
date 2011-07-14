// Binary to Decimal, Hexadecimal and Octal conversion program
// tested with Pelles C vegaseat 15dec2004
 
#include <stdio.h>
#include <string.h>
 
int bin2dec(char *bin);
 
int main()
{
    char bin[80] = "";
    char *p;
    int dec;

    while(strcmp(bin,"0"))
    {
        printf("\n Enter a binary number (just 0 to EXIT): ");
        fgets(bin, sizeof(bin), stdin);
        // check for and remove trailing \n
        if ((p = strchr(bin,'\n')) != NULL)
        {
            *p = '\0';
        }
        dec = bin2dec(bin);
        if (dec) printf("\nDecimal = %d Hexadecimal = 0x%04X Octal = 0%o\n",dec,dec,dec);
    }

    getchar(); // wait
    return 0;
}

// convert a binary string to a decimal number, returns decimal value
int bin2dec(char *bin)
{
    int b, k, m, n;
    int len, sum = 0;

    len = strlen(bin) - 1;
    for(k = 0; k <= len; k++)
    {
        n = (bin[k] - '0'); // char to numeric value
        if ((n > 1) || (n < 0))
        {
            puts("\n\n ERROR! BINARY has only 1 and 0!\n");
            return (0);
        }
        for(b = 1, m = len; m > k; m--)
        {
            // 1 2 4 8 16 32 64 ... place-values, reversed here
            b *= 2;
        }
        // sum it up
        sum = sum + n * b;
        printf("%d*%d + ",n,b); // uncomment to show the way this works
    }
    return(sum);
}
