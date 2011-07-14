#include <stdio.h>
#include <stdlib.h>
#define B_SIZE 4;

struct b_number {
    unsigned long int size;
    unsigned long int* block_list;
    int id;
};
struct link {
    unsigned int flags; // flags
    /* bit    "0x1" 0         "0x2" 1        "0x4" 2
     *      (carry_out) (init_sum finished) (totally done)
     */
};
struct chain {
    struct link* links;
    struct b_number* n1;
    struct b_number* n2;
    struct b_number* sum;
};

int c_add( long unsigned int* arg1, long unsigned int* arg2, long unsigned int* sum );
void b_add_one( struct b_number* small_num, struct b_number* large_num, struct b_number* sum);

/*
 * Add n1 + n2 = sum
 * This part can be threaded. I'm just going to impemelent a basic carry adder.
 * b_add will decide how large the sum ds needs to be.
 *
 */
struct b_number* b_add( struct b_number* n1, struct b_number* n2 ){
    struct b_number* large_num;
    struct b_number* small_num;
    struct b_number* sum;
    if( n1->size < n2->size ){
        small_num = n1;
        large_num = n2;
    } else {
        small_num = n2;
        large_num = n1;
    }
    // Creat the new big number
    sum = (struct b_number*) malloc(sizeof(struct b_number));
    sum->block_list = calloc(large_num->size,sizeof(unsigned long int));
    sum->size = large_num->size;
    b_add_one( small_num, large_num, sum );
    return sum;

}

/*
 * Add small_num to large_num. Store in sum.
 * @precondition: sum needs to be the same size or larger than large_num.
 *
 */
void b_add_one( struct b_number* small_num, struct b_number* large_num, struct b_number* sum){
    unsigned long int done,carry,i,temp_carry,extra;
    struct chain add_chain;
    /*
     * n14 n13 n12 n11
     * +   n23 n22 n21
     * _______________
     */
    // Decide who is bigger.

    add_chain.links = calloc(large_num->size, sizeof(struct link));
    /*
     * Set big_number vars in link_chian.
     */
    add_chain.n1 = large_num; // Large on top
    add_chain.n2 = small_num;
    add_chain.sum = sum;
    /*
     * You only need to add up to the smaller number's size. The new number needs to
     * be as big as the biggest number and can be identical to the larger number
     * where the smaller number doesn exist
     */
    carry = done = 0;
    while ( done < small_num->size ){ // Until we are at a stable state
        for( i = 0; i < small_num->size ; i++ ) {
            if( !( add_chain.links[i].flags & 0x2 ) ) { // Check if initial sum is complete
                // Do initial sum
                carry = c_add(  &add_chain.n1->block_list[i], \
                                &add_chain.n2->block_list[i], \
                                &add_chain.sum->block_list[i] );
                add_chain.links[i].flags |= 0x2; // Set init_sum finished
                if ( carry )
                    add_chain.links[i].flags |= 0x1; // Set carry out bit
            }
            if( i == 0 ) { // First link. He is done.
                add_chain.links[i].flags |= 0x4;
                done++;
                continue;
            }
            if ( add_chain.links[i-1].flags & 0x4 ) { // Is the link befor us totally done?
                // prev link is done. Check carry.
                if ( add_chain.links[i-1].flags & 0x1 ) { // Was there a carry on the previous?
                    temp_carry = 1;
                    carry = c_add( &temp_carry, &sum->block_list[i], &sum->block_list[i] );
                    if ( carry )
                        add_chain.links[i].flags |= 0x1; // Set carry out bit
                }
                // Check flags. And say we are finished
                if( add_chain.links[i].flags & 0x2 ) { // Being paranoid...
                    add_chain.links[i].flags |= 0x4;
                    done++;
                }
            }
        } // end for
    } // end while

    if( add_chain.links[large_num->size-1].flags & 0x1 ){ // Does the last link in the chain have a carry?
        // Create new block in sum, inc size
        printf("Overflow in last block...\n");
    }
    /*
     * If n1 and n2 were different sizes and there was no carry in the 
     * upper blocks, we need to copy the rest of bigger number into the
     * sum. Pick up where the loop above left off. For now (with no linked
     * list, a memcpy will work.
     */
    if (large_num->size > small_num->size){
        extra = large_num->size - small_num->size;
        memcpy( &add_chain.sum->block_list[small_num->size], \
                &large_num->block_list[small_num->size], \
                extra*sizeof(unsigned long int) );
        /* While there is still a cary in the upper blocks of sum, add.
         * I can't see how this could be done in parallel.
         */
        for( i=small_num->size ; i<large_num->size; i++ ){
            temp_carry = 1; //Paranoid
            if ( add_chain.links[i-1].flags & 0x1 ) {
                carry = c_add( &temp_carry, &sum->block_list[i], &sum->block_list[i] );
                if ( carry )
                    add_chain.links[i].flags |= 0x1; // Set carry out bit
            } else {
                break;
            }
        }
        // We need this much overflow
        if( extra > 0 ){
        }
    }

}

/*
 * Add two long int's numbers. To be used in big num add, flags the user if there is a "carry" (c_add)
 * @return  1 if there was an overflow
 *          0 if there was not an overflow
 *
 */
int c_add( long unsigned int* arg1, long unsigned int* arg2, long unsigned int* sum ){
    int carry = 0;
    int result;
    __asm__ (   "addl %%ebx, %%ecx;"
                "movl $0x0, %%eax;"
                "adc $0x0, %%eax;"
                : "=c" (*sum), "=a" (carry)
                : "b" (*arg1), "c" (*arg2)
    );

    result = carry ? 1 : 0;
    return result;
}
/*
 * Create a new struct that is a copy of the original. Pass 1 if you want
 * the data copied over, 0 if you just want the same size struct.
 */

struct b_number* clone( int copy, struct b_number* orig ){
    struct b_number* new;
    new = (struct b_number*) calloc( 1, sizeof(struct b_number)); 
    new->block_list = (unsigned long int*) malloc(sizeof(unsigned long int) * orig->size );
    new->size = orig->size;
    if( copy )
        memcpy( new->block_list, orig->block_list, sizeof(unsigned long int) * orig->size );
    return new;
}

/*
 * Pass in two bn's possible created by clone() and copy_bn
 * will make the two identical. First copy (int)-> Second
 */
void bn_copy( struct b_number* n1, struct b_number* n2 ) {
    n2->size = n1->size;
    n2->id = n1->id;
    memcpy(&n2->block_list[0],&n1->block_list[0],(n1->size)*sizeof(unsigned long int));
}

/*
 * Convert a number into it's two's compliment.
 */
void two_comp( struct b_number** n ){
    struct b_number temp;
    struct b_number *new;
    unsigned long int i;
    temp.size = 1;
    temp.block_list = malloc(sizeof(unsigned long int));
    temp.block_list[0] = 1L;
    // Thread this shit!
    for( i=0; i< (*n)->size; i++ ){
        (*n)->block_list[i] = ~( (*n)->block_list[i] ); // Not all the values
    }
    new = b_add( *n, &temp );
    free((*n)->block_list);
    free(*n);
    *n = new;
    
}

/*
 * multiply n1 by mult. Right now only the first unsigned long int is used.
 * Eventually it shuould use the entire bit b_number.
 * @Return n1 multiplied by mult->block_list[0]
 */
void b_mult( struct b_number *mult, struct b_number *n1 ){
    int i;
    struct b_number *b1;
    struct b_number *b2;

    b1 = clone( 1, n1 );
    b2 = clone( 1, n1 );
    // b1 and b2 are clones of n1
    // b2 stays constant ( the original value of n1 )
    // n1 becomes the sum of b1 and b2.
    // n1 is copied into b1
    for( i=1; i< mult->block_list[0]; i++ ){
        b_add_one( b1, b2, n1 );
        //n1 = b1 + b2
        bn_copy(n1,b1);  
    }
}
