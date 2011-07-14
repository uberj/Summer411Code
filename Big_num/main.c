#include <stdio.h>
#include "read.c"

int main( int argc, char **argv ) {
    struct b_number* bn1;
    struct b_number* bn2;
    struct b_number* bn3;
    struct b_number* bn4;
    struct b_number* sum;
    char fname[30];
    FILE *fp;
    // Generate our two numers.
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

    // Test's
    
    //Two's comp
    /*
    printf("Calcing two's comp...\n");
    printf("befor...\n");
    print_bn(bn1);
    bn3 = clone(1,bn1); // Make copy to add to two's comp
    two_comp(&bn1);
    printf("after...\n");
    print_bn(bn1);
    printf("Adding one (should see this):\n: 0: 0\nActually see:\n");
    bn4 = b_add( bn1, bn3 );
    print_bn(bn4);
    */
    
     
    /*
    //Add test
    printf("Adding test...\n");
    printf("bn1 is:\n");
    print_bn(bn1);
    printf("bn2 is:\n");
    print_bn(bn2);
    sum = b_add( bn2, bn1 );
    printf("Sum is:\n");
    print_bn(sum);
    */

    //Mult test
    printf("Mult test...\n");
    printf("mult is:\n");
    print_bn(bn1);
    printf("bn2 is:\n");
    print_bn(bn2);
    b_mult( bn1, bn2 );
    printf("Prodect is:\n");
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

