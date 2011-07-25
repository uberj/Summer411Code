#include "read.h"
#define BUFF_LEN 200 // For printing

static char num_buffer[BUFF_LEN];

void convert_bn_to_file( FILE *fp, struct b_number** bn ){
    struct b_number* ten; // Big number used for expn
    struct b_number* n1;
    struct b_number* n2;
    struct b_number* n3;
    struct b_number* n4;
    char num;
    char newline = '\n';
    int i = BUFF_LEN - 1;

    /*
     * Initialize the file
     */
    fseek( fp, 0L, SEEK_SET );
    /*
     * Initialize our needed helper numbers
     */
    n1 = clone( 1, *bn ); // We are gonig to mutate bn
    ten = clone( 0, *bn ); // Needed for fast_div_one
    bzero( ten->block_list , ten->size * sizeof( uint32_t ));
    ten->block_list[0] = 10;

    n3 = (struct b_number *)malloc(sizeof(struct b_number));
    n3->size = 1;
    n3->block_list = (uint32_t*) malloc(sizeof(uint32_t));

    n2 = clone( 1, n1 );

    while( b_compare( ten, n1 ) >= 0 ){
        bzero( n2->block_list , n2->size * sizeof( uint32_t ));
        bzero( n3->block_list , n3->size * sizeof( uint32_t ));
        b_fast_div_one( n1, ten, n2, n3 );
        num = n3->block_list[0] + '0';
        n4 = n1;
        n1 = n2;
        n2 = n4;
        num_buffer[i] = num;
        i--;
    }
    num = (char)n1->block_list[0] + '0';
    num_buffer[i] = num;
    fwrite( &num_buffer[i], 1, BUFF_LEN-i, fp);
    fwrite( &newline, 1 , 1 , fp );

}

/*
 * Take a file (fp) and convert it's ascii representation (base 10)
 * of a number into a binary representation and store it in the bn
 * struct.
 */
void convert_file_to_bn( FILE *fp , struct b_number** bn ){
    char num;
    int f_size,i;
    long double result,max;
    uint32_t size;
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
    max = pow(10,f_size+1);
    result = ( (long double)log(max)/(long double)log(2) ) + 1;
    size = result / (sizeof(uint32_t)*8);
    n1->size = size+1;
    //printf("Optimal size: %lu\n",n1->size);
    n1->block_list = (uint32_t*) calloc(n1->size,sizeof(uint32_t));

    // Make copies
    n2 = clone( 1, n1 );

    // Set up temp
    temp.size = 1;
    temp.block_list = malloc(sizeof(uint32_t));

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
        temp.block_list[0] = (uint32_t) num - '0';
        n1 = b_add( n2, &temp );
        free(n2->block_list);
        free(n2);
        if( f_size == 0 )
            break;
        n2 = clone( 1, n1 );
        //printf("Added new number: ");
        //print_bn( n1 );
        //printf("Mult by 10...\n");
        /*
         * n2 is the cumilator. add n1 to 10 times (9 in reality due to it already being at n1*1.
         */
        for( i=0; i<9; i++ ){
            n3 = b_add( n1, n2 );
            free(n2->block_list);
            free(n2);
            n2 = n3;
       //     printf("Number after add\n");
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
