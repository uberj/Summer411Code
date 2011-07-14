#include <stdio.h>
#include "read.c"

int main( int argc, char **argv ) {
    struct b_number* bn1;
    struct b_number* bn2;
    struct b_number* sum;
    char fname[30];
    FILE *fp;
    strcpy(fname,"ascii_big_num1.txt");
    fp = fopen(fname, "r");
    convert_file_to_bn( fp, &bn1 );
    fclose(fp);
    print_bn(bn1);
    // Second number
    strcpy(fname,"ascii_big_num2.txt");
    fp = fopen(fname, "r");
    convert_file_to_bn( fp, &bn2 );
    fclose(fp);
    print_bn(bn2);
    
    sum = b_add( bn2, bn1 );
    printf("Sum is:\n");
    print_bn(sum);
    printf("bn2 is:\n");
    print_bn(bn2);

    return 0;
    // Do add.

    /*
    unsigned long int i,sum,c_sum;
    i=1;
    sum=0;
    c_sum = 2;
    c_add(&c_sum,&i,&sum);
    printf("Sum was: %lu\n",sum);
    return 0 ;
    */
}

