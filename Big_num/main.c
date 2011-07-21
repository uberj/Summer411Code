#include <stdio.h>
#include "read.h"
#include "big_num.h"


int main( int argc, char **argv ) {
    struct b_number* bn1;
    struct b_number* bn2;
    FILE *fp,*n1,*n2,*expect,*actual,*remain,*quot;

    // run_test <flag> <bn1> <bn2> <expect> <actual> <remain> (remain only with division)
    n1 = fopen(argv[2], "r");
    n2 = fopen(argv[3], "r");
    expect = fopen(argv[4], "r");

    if( !( n1 && n2 && expect && actual ) ) {
        printf("File could not be opened\n");
        return 2;
    }
    /*
     * Make numbers
     */
    convert_file_to_bn( n1, &bn1 );
    close(n1);
    convert_file_to_bn( n2, &bn2 );
    close(n2);

    switch( (char)argv[1][1] ){
        case 'A':
            actual = fopen(argv[5], "w+"); // expected remainder in Division test
            add_test( bn1, bn2, actual );
            close(actual);
            break;
        case 'M':
            actual = fopen(argv[5], "w+"); // expected remainder in Division test
            mult_test( bn1, bn2, actual );
            close(actual);
            break;
        case 'D':
            quot = fopen(argv[6], "w+");
            remain = fopen(argv[7], "w+");
            div_test( bn1, bn2, quot, remain );
            close(remain);
            close(quot);
            break;
    }
    // Generate our two numers.
    /*
    fp = fopen(fname, "r");
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
    */

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
    
    printf("Adding test 2 ...\n");
    printf("bn1 is:\n");
    print_bn(bn1);
    printf("bn2 is:\n");
    print_bn(bn2);
    b_add_one( bn2, bn1, bn2 );
    printf("Sum is:\n");
    print_bn(bn2);
    */

    /*
    //Mult test
    printf("Mult test...\n");
    printf("mult is:\n");
    print_bn(bn1);
    printf("bn2 is:\n");
    print_bn(bn2);
    b_mult( bn1, bn2 );
    printf("Prodect is:\n");
    print_bn(bn2);
    */

    /*
    // Inc test
    printf("Inc Test...\n");
    printf("Number is:\n");
    print_bn(bn1);
    printf("Adding one...\n");
    b_inc(bn1);
    printf("Sum:\n");
    print_bn(bn1);
    printf("Doing 100 incriments...\n");
    for(i=0;i<100;i++){
        b_inc(bn1);
    }
    print_bn(bn1);
    */
    /*
    // Inc test
    printf("Dec Test...\n");
    printf("Number is:\n");
    print_bn(bn1);
    printf("Subtracting one...\n");
    b_dec(bn1);
    printf("Sum:\n");
    print_bn(bn1);
    printf("Doing 100 dec...\n");
    for(i=0;i<100;i++){
        b_dec(bn1);
    }
    print_bn(bn1);
    */
    
    /*
    printf("Compare test...\n");
    printf("Number 1\n");
    print_bn(bn1);
    printf("Number 2\n");
    print_bn(bn2);
    compare = b_compare(bn1, bn2);
    if(compare == -1 )
        printf("Number 1 is bigger\n");
    else if (compare == 1)
        printf("Number 2 is bigger\n");
    else
        printf("Number 1 and 2 are equal\n");
    */

    /*
    printf("Divide test...\n");
    printf("Number 1\n");
    print_bn(bn1);
    printf("Number 2\n");
    print_bn(bn2);
    b_divide( bn1, bn2, &bn3, &bn4 );
    printf("Quotent:\n");
    print_bn(bn3);
    printf("Remainder:\n");
    print_bn(bn4);
    */

    /*
    printf("sb_check test...\n");
    printf("Number 1\n");
    print_bn(bn1);
    sb_check(bn1,&block,&bit);
    printf("Number 2\n");
    print_bn(bn2);
    sb_check(bn2,&block,&bit);
    pos = b_msb(bn1);
    printf("bn1 MSB found at %lu\n",pos);
    pos = b_msb(bn2);
    printf("bn2 MSB found at %lu\n",pos);
    */

    /*
    printf("lshift test...\n");
    print_bn(bn1);
    printf("Shifting 10 times\n");
    for(i=0; i<33;i++){
        b_lshift(bn1);
        print_bn(bn1);
    }
    */
    
    /*
    printf("rshift test...\n");
    print_bn(bn1);
    printf("Shifting 10 times\n");
    for(i=0; i<100;i++){
        b_rshift(bn1);
        print_bn(bn1);
    }
    */

    /*
    //EXP test
    printf("Exp test...\n");
    printf("base is:\n");
    print_bn(bn1);
    printf("exp is:\n");
    print_bn(bn2);
    _b_expn_test( bn1, bn2, &bn3 );
    printf("Result is:\n");
    print_bn(bn3);
    */

    /*
    printf("Fast Divide test...\n");
    printf("Numerator\n");
    print_bn(bn1);
    printf("Denominator\n");
    print_bn(bn2);
    b_fast_div( bn1, bn2, &bn3, &bn4 );
    printf("Quotent:\n");
    print_bn(bn3);
    printf("Remainder:\n");
    print_bn(bn4);

    convert_bn_to_file(actual,&bn1);

    */

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

