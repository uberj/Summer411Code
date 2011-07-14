#include <stdio.h>
#include <string.h>
#include <math.h>
#include "big_num.c"


void print_bn( struct b_number *bn );

/*
 * Take a file (fp) and convert it's ascii representation (base 10)
 * of a number into a binary representation and store it in the bn
 * struct.
 */
void convert_file_to_bn( FILE *fp , struct b_number** bn ){
    char num;
    long double result, x;
    int f_size,i;
    struct b_number temp;
    struct b_number* n1;
    struct b_number* n2;
    struct b_number* n3;

    /* Do a base conversion! yeah! */
    //First figure out how many blocks we will need.
    fseek( fp, 0L, SEEK_END );
    f_size = ftell( fp );
    f_size -= 1;
    fseek( fp, 0L, SEEK_SET );


    n1 = (struct b_number*) malloc(sizeof(struct b_number));
    x = 2;
    result = log(f_size) / log(x);
    printf("Optimal size: %Lg\n",result);
    n1->size = result / (int) 1;
    n1->block_list = (unsigned long int*) calloc(n1->size,sizeof(unsigned long int));

    // Make copies
    n2 = clone( 1, n1 );

    // Set up temp
    temp.size = 1;
    temp.block_list = malloc(sizeof(unsigned long int));

    /*
     * For now just add two numbers. Later when big num add works use that.
     */
    for( num = fgetc(fp); num != '#' ; num = fgetc(fp) ) {
        f_size --; // We need to stop one before the end. This contains number of chars found earlier.
        //temp = temp*10 + num - '0';
        // No multiplication, only addition.
        /*
         * As far as I'm concerned this is a hack until I can write a
         * multiplication function.
         */
        temp.block_list[0] = (unsigned long int) num - '0';
        n1 = b_add( n2, &temp );
        free(n2->block_list);
        free(n2);
        if( f_size == 0 )
            break;
        n2 = clone( 1, n1 );
        printf("Added new number: ");
        print_bn( n1 );
        printf("Mult by 10...\n");
        /*
         * n2 is the cumilator. add n1 to 10 times (9 in reality due to it already being at n1*1.
         */
        for( i=0; i<9; i++ ){
            n3 = b_add( n1, n2 );
            free(n2->block_list);
            free(n2);
            n2 = n3;
        //    print_bn( n2 );
        }

    }
    *bn = n1;
}

void print_bn( struct b_number *bn ){
    int i;
    for( i=0; i < bn->size; i++ ){
        printf(": %lu",bn->block_list[i]);
    }
    printf("\n");
}
