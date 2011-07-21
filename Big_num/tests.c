#include "big_num.h"
#include "read.h"
#include "tests.h"

void add_test( struct b_number* n1, struct b_number* n2, FILE *fp ) {
    struct b_number *sum;

    //Add test
    /*
    printf("bn1 is:\n");
    print_bn(n1);
    printf("bn2 is:\n");
    print_bn(n2);
    */
    sum = b_add( n2, n1 );
    //printf("Sum is:\n");
    convert_bn_to_file(fp, &sum);
    //print_bn(sum);
    free_bn(sum);

}
            
void mult_test( struct b_number* n1, struct b_number* n2, FILE *fp ) {
    b_mult( n2, n1 );
    convert_bn_to_file( fp, &n1 );
}
