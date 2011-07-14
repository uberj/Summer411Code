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

/*
 * Add n1 + n2 = sum
 * This part can be threaded. I'm just going to impemelent a basic carry adder.
 * b_add will decide how large the sum ds needs to be.
 *
 */
struct b_number* b_add( struct b_number* n1, struct b_number* n2 ){
    unsigned long int done,carry,i,temp_carry;
    struct b_number* large_num;
    struct b_number* small_num;
    struct b_number* sum;
    struct chain add_chain;
    /*
     * n14 n13 n12 n11
     * +   n23 n22 n21
     * _______________
     */
    // Decide who is bigger.
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
    while ( done < small_num->size ){
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
                    // Check flags.
                    if( add_chain.links[i].flags & 0x2 ) // Being paranoid...
                        add_chain.links[i].flags |= 0x4;
                        done++;
                }
            }
            if( add_chain.links[large_num->size-1].flags & 0x1 ){ // Does the last link in the chain have a carry?
                // Create new block in sum, inc size
                printf("Overflow in last block...\n");
            }
        } // end for
    } // end while

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

    return sum;

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
 * Calculate the two's compliment of a number.
 */
struct b_number* two_comp( struct b_number n );
