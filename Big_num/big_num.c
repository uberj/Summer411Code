#include "big_num.h"
#include "read.h"
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
    // Get the new big number
    sum = (struct b_number*) malloc(sizeof(struct b_number));
    sum->block_list = calloc(large_num->size,sizeof(uint32_t));
    sum->size = large_num->size;
    b_add_one( small_num, large_num, sum );
    return sum;

}

/*
 * Add small_num to large_num. Store in sum.
 * @Precondition: Sum needs to be the same size or larger than large_num.
 * @Note: b2 + b1 = b2 <- this will work
 */
void b_add_one( struct b_number* small_num, struct b_number* large_num, struct b_number* sum){
    uint32_t done,carry,i,temp_carry,extra;
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
     * where the smaller number doesn't exist
     */
    carry = done = 0;
    while ( done < small_num->size ){ // Until we are at a stable state
        #pragma omp parallel for
        for( i = 0; i < small_num->size ; i++ ) {
            if( !( add_chain.links[i].flags & 0x2 ) ) { // Check if initial sum is complete.
                // Do initial sum.
                carry = c_add(  &add_chain.n1->block_list[i], \
                                &add_chain.n2->block_list[i], \
                                &add_chain.sum->block_list[i] );
                add_chain.links[i].flags |= 0x2; // Set init_sum finished.
                if ( carry )
                    add_chain.links[i].flags |= 0x1; // Set carry out bit.
            }
            if( i == 0 ) { // First link. He is done.
                add_chain.links[i].flags |= 0x4;
                done++;
                continue;
            }
            if ( add_chain.links[i-1].flags & 0x4 ) { // Is the link befor us totally done?
                // Prev link is done. Check carry.
                if ( add_chain.links[i-1].flags & 0x1 ) { // Was there a carry on the previous?
                    temp_carry = 1;
                    carry = c_add( &temp_carry, &sum->block_list[i], &sum->block_list[i] );
                    if ( carry )
                        add_chain.links[i].flags |= 0x1; // Set carry out bit.
                }
                // Check flags. And say we are finished.
                if( add_chain.links[i].flags & 0x2 ) { // Being paranoid...
                    add_chain.links[i].flags |= 0x4;
                    done++;
                }
            }
        } // end for
    } // end while

    /*
     * If this were ever moved onto a cluster, here is where node to node communication would have to be initiated.
     */
    /*
    if( add_chain.links[large_num->size-1].flags & 0x1 ){ // Does the last link in the chain have a carry?
        // Create new block in sum, inc size
        //printf("Overflow in last block...\n");
    }
    */
    /*
     * If n1 and n2 were different sizes and there was no carry in the
     * upper blocks, we need to copy the rest of the bigger number into the
     * sum. Pick up where the loop above left off. For now (with no linked
     * list, a memcpy will work.)
     */
    if (large_num->size > small_num->size){
        extra = large_num->size - small_num->size;
        memcpy( &add_chain.sum->block_list[small_num->size], \
                &large_num->block_list[small_num->size], \
                extra*sizeof(uint32_t) );
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
        // We need this much overflow.
        if( extra > 0 ){
        }
    }

}

/*
 * Add two long int numbers. To be used in big num add, flags the user if there is a "carry" (c_add)
 * @return  1 if there was an overflow
 *          0 if there was not an overflow
 *
 */
int c_add( uint32_t* arg1, uint32_t* arg2, uint32_t* sum ){
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
    new->block_list = (uint32_t*) malloc(sizeof(uint32_t) * orig->size );
    new->size = orig->size;
    if( copy )
        memcpy( new->block_list, orig->block_list, sizeof(uint32_t) * orig->size );
    return new;
}

/*
 * Pass in two bn's (possibly) created by clone() and copy_bn
 * will make the two identical. First copy (int)-> Second
 */
void bn_copy( struct b_number* n1, struct b_number* n2 ) {
    n2->size = n1->size;
    n2->id = n1->id;
    memcpy(&n2->block_list[0],&n1->block_list[0],(n1->size)*sizeof(uint32_t));
}

/*
 * Convert a number into it's two's compliment.
 * @Postcondition: *n will be different.
 */
void two_comp( struct b_number** n ){
    uint32_t i;
    for( i=0; i< (*n)->size; i++ ){
        (*n)->block_list[i] = ~( (*n)->block_list[i] ); // Not all the values.
    }
    b_inc(*n);
    return;
}

/*
 * @Preconditions. It is up to the programmer to pass the smaller of the two numbers
 * as the first parameter and the larger as the second. This will speed up preformance.
 * If one wanted to, they could write a wrapper to automatically do this.
 * @Return n1 multiplied by mult
 *
 */

void b_mult( struct b_number *mult, struct b_number *n1 ){
    struct b_number *b1;
    struct b_number *b2;
    struct b_number *p_mult; // Copy of mult
    struct b_number zero;
    zero.size = 1;
    zero.block_list = malloc(sizeof(uint32_t));
    zero.block_list[0] = 0;
    p_mult = clone( 1, mult);


    b1 = clone( 1, n1 );
    b2 = clone( 1, n1 );
    // b1 and b2 are clones of n1
    // b2 stays constant ( the original value of n1 )
    // n1 becomes the sum of b1 and b2.
    // n1 is copied into b1
    b_dec( p_mult ); // We start at mult*1
    while( b_compare( p_mult, &zero ) == -1 ) {
        b_add_one( b1, b2, n1 );
        //n1 = b1 + b2
        bn_copy(n1,b1);
        //n1 -> b1
        b_dec( p_mult );
    }
}

/*
 * Compare two numbers
 * @return:
 *  0 equal
 *  -1 first one is bigger
 *  1 second one is bigger
 */
int b_compare( struct b_number *n1, struct b_number *n2 ){
    int i;
    struct b_number *small_num; // These are refereing to bit size, not value.
    struct b_number *large_num;

    if( n1->size < n2->size ){
        small_num = n1;
        large_num = n2;
    } else {
        small_num = n2;
        large_num = n1;
    }

    // If anything exist in large_num's upper blocks, it's value is larger.
    for( i = large_num->size-1 ; i >= small_num->size ; i-- ){
        // Starting at the msb of large_num
        if(large_num->block_list[i] != 0){
            // Determine who large_num is.
            if (large_num == n1) {
                //large_num is n1
                return -1;
            } else {
                //large_num is n2
                return 1;
            }
        }
    }
    // Now look for a bigger block in either of the two numbers. If no difference is found
    // the two numbers are equal.
    for( i = small_num->size -1; i>=0; i-- ) {
        if( n1->block_list[i] > n2->block_list[i] ) {
            return -1;
        } else if( n2->block_list[i] > n1->block_list[i] ){
            return 1;
        }
    }
    return 0;
}

/*
 * Incriment n by one.
 */
void b_inc( struct b_number *n ){
    int carry,i;
    uint32_t one;
    one = 1;

    for( i=0; i<n->size; i++ ){
        carry = c_add( &one, &n->block_list[i], &n->block_list[i]);
        if(!carry)
            break;
    }

}

/*
 * Decriment n by one.
 */
void b_dec( struct b_number *n ){
    int i;

    for( i=0; i<n->size; i++ ){
        if(n->block_list[i] == 0) {
            n->block_list[i] = ~n->block_list[i];
        } else {
            n->block_list[i]--;
            break;
        }
    }

}

/*
 * Shift to the right >> (Divide by 2)
 */
void b_rshift( struct b_number *n ){
    int i,prev_carry,carry;
    uint32_t lsb = 1L;
    uint32_t msb = 0x80000000;

    prev_carry = carry = 0;
    for( i=n->size-1; i >= 0; i-- ){
        if (n->block_list[i] == 0) {
            if ( prev_carry )
                n->block_list[i] |= msb;
            prev_carry = carry = 0;
            continue;
        } else {
            if ( lsb & n->block_list[i] )
                carry = 1;
            // Shift it up.
            n->block_list[i] = n->block_list[i] >> 1;
            if ( prev_carry )
                n->block_list[i] |= msb;
            prev_carry = carry;
            carry = 0;
        }
    }
}
/*
 * Shift to the left << (Multiply by 2)
 */
void b_lshift( struct b_number *n ){
    int i,prev_carry,carry;
    uint32_t msb = 0x80000000;

    prev_carry = carry = 0;
    for( i=0; i<n->size; i++ ){
        if (n->block_list[i] == 0) {
            if ( prev_carry )
                n->block_list[i]++;
            prev_carry = carry = 0;
            continue;
        } else {
            if ( msb & n->block_list[i])
                carry = 1;
            // Shift it up.
            n->block_list[i] = n->block_list[i] << 1;
            if ( prev_carry )
                n->block_list[i]++;
            prev_carry = carry;
            carry = 0;
        }
    }
}

/*
 * Exponentiate: The hacked non-optimized version (repitition and multiplication).
 * If you pass in a result with an existing struct make sure it's size > 0. b_expn
 * will not touch it and just use the existing struct if result->size > 0. If it
 * is zero a result struct of arbitrary size should be allocated.
 *
 * @precondition: result is zeroed out and a reasonable size.
 */

void b_expn( struct b_number *base, struct b_number *exp, struct b_number *result ) {
    struct b_number zero;
    zero.size = 1;
    zero.block_list = malloc(sizeof(uint32_t));
    zero.block_list[0] = 0;

    result->block_list[0] = 1;
    if( result->size == 0 )
        printf("1b_expn sanity check failed\n");

    while( b_compare( &zero, exp ) != 0 ){
        b_mult( base, result );
        b_dec( exp );
    }
}

void _b_expn_test ( struct b_number *base, struct b_number *exp, struct b_number **result) {
    *result = (struct b_number*)malloc(sizeof(struct b_number));
    (*result)->size = 8; //Something reasonable
    (*result)->block_list = (uint32_t*) calloc((*result)->size,(*result)->size*sizeof(uint32_t));
    b_expn( base, exp, *result );
}

/*
 * DEPRICATED
 * Division. Pass b_number's numer (numerator) and denom (denominator).
 * Also pass b_number pointers quot (quotent) and remainder (I think you get the picture).
 * @returns:
 *  numer/denom = quot, remainder
 * @post-cond:
 *  Numer is mutated. There should probably be a wrapper written.
 *  User is responsible for freeing quot and remainder.
 */

void b_divide(  struct b_number* numer, struct b_number* denom, \
                struct b_number** quot, struct b_number** remainder ) {


    (*quot) = (struct b_number *) malloc(sizeof(struct b_number));
    (*remainder) = (struct b_number *) malloc(sizeof(struct b_number));

    // q_size <= sizeof( numer ) - sizeof( denom ) + 1
    // ^ random theory
    (*quot)->size = numer->size - denom->size + 1;
    (*remainder)->size = denom->size;

    //Allocate block_list's
    (*quot)->block_list = (uint32_t*) calloc((*quot)->size,(*quot)->size*sizeof(uint32_t));
    (*remainder)->block_list = (uint32_t*) calloc((*remainder)->size,(*remainder)->size*sizeof(uint32_t));

    b_divide_one( numer, denom, *quot, *remainder );

}

void b_divide_one(  struct b_number* numer, struct b_number* denom, \
                struct b_number* quot, struct b_number* remainder ) {

    struct b_number* twos_denom; // The denomenators two's compliment used for subtraction.
    struct b_number* sum; // temp sum var.
    struct b_number* swap; // Pretty much a swap var. :)
    // Sanity checking
    if( b_compare(numer,denom) == 1) { // Is the denom > numer?
        return;
    } else if ( b_compare(numer,denom) == 0) { // Are they equal?
        quot->block_list[0] = 1;
        return;
    }

    twos_denom = clone( 0, numer ); // Make a copy. It has to be size of numer because we need the over flow bit to work it's magic.
    // Now copy in all the bits in denom .
    memcpy(twos_denom->block_list, denom->block_list, numer->size * sizeof(uint32_t));
    two_comp(&twos_denom); // Convert to two's compliment.
    sum = clone( 0, numer);


    while( b_compare( numer, denom ) <= 0 ) { // while numer > denom
        /*
         * TODO This is going to be done very clumsily with the old add function.
         * Replace this when the new add is written.
         */
        b_add_one( numer, twos_denom, sum );
        swap = numer;
        numer = sum;
        memset(swap->block_list,0,swap->size); //Paranoid again
        sum = swap;
        b_inc(quot);

    }

    // Move the rest of numer into remainer.
    memcpy(remainder->block_list,numer->block_list,(remainder->size)*sizeof(uint32_t));
}


/*
 * sb_check: A function to help with b_fast_div. To make division faster
 * we need to know when the number we are shifting is just a little bit
 * larger than the number it's going to be subtracted from. I.e. If the
 * number being divided is n bits, we want to know when the number being
 * shifted is n-1 bits and stop squaring it.
 *
 * This function's job is to return the posistion of MSB.
 *
 */
void sb_check( struct b_number *n, uint32_t *block, uint32_t *bit ){
    uint32_t i,j,index;
    // Find the highest block that is not zero
    for( i = (n->size)-1; i>=0; i--){
        if( n->block_list[i] > 0 )
            break;
    }
    // We found a non zero block. Check were the MSB is
    index = pow(2,sizeof(uint32_t)*8-1);
    for( j = (sizeof(uint32_t)*8)-1; j >= 0; j-- ){
        if( index & n->block_list[i] )
            break;
        else
            index = index / 2;
    }
    //printf("SB found in In block %lu, bit %lu\n",i,j);
    *block = i;
    *bit = j;
}

/*
 * A little wrapper function for sb_check.
 * WARNING: If a number is larger than ~4.2 (32 bit's) this function will
 * return incorrect results.
 *
 */
uint32_t b_msb( struct b_number* n) {

    uint32_t n_size,n_bit;// In bits
    sb_check( n, &n_size, &n_bit);
    return n_size * 32 + n_bit;
}


void b_fast_div(  struct b_number* numer, struct b_number* denom, \
                struct b_number** quot, struct b_number** remainder ) {

    struct b_number zero;
    int sanity;
    zero.size = 1;
    zero.block_list = malloc(sizeof(uint32_t));
    zero.block_list[0] = 0;

    if ( ! b_compare( &zero, denom ) ){ // Dividing by zero
        printf("ULTIMATE SIN!!!!!!\n");
        return; //666 ur going to math hell.
    }
    *quot = clone( 0, numer );
    bzero( (*quot)->block_list , (*quot)->size * sizeof( uint32_t ));
    *remainder = clone( 0, denom );
    bzero( (*remainder)->block_list , (*remainder)->size * sizeof( uint32_t ));

    sanity = b_compare( numer, denom );
    if( sanity == 1 ) { // denom > numer
        memcpy((*remainder)->block_list, denom->block_list, (*remainder)->size * sizeof( uint32_t ));
        return;
    } else if ( sanity == 0 ) { // denom == numer
        (*quot)->block_list[0] = 1;
        return;
    }

    b_fast_div_one( numer, denom, *quot, *remainder);
}

/*
 * Optimized division - It's fast
 * @Preconditions.
 * Numer is a valid b_number.
 * Denom is non-zero b_number.
 * @Postconditions
 * quot will point to a b_number with the correct quotent.
 * remainder will point to a b_number with the correct remainder.
 */

void b_fast_div_one(  struct b_number* numer, struct b_number* denom, \
                struct b_number* quot, struct b_number* remainder ) {
    uint32_t shift, adjust;
    uint32_t msb_n, msb_d;
    int i;
    /*
     * msb = most significant bit
     * msb_[n,d] = constants, only calculated once
     * msb_[n,d]p = numerator and denominator prime. These change and are recomputed.
     */
    struct b_number* n_pr; //numerator prime
    struct b_number* d_pr; //denominator prime
    struct b_number* temp_exp; // Used as result for b_expn
    struct b_number two; // Big number used for expn
    struct b_number b_shift; // Big number used for expn
    msb_n = b_msb(numer);
    msb_d = b_msb(denom);

    /*
     * Intialize all utility Big nums
     */

    n_pr = clone( 1, numer );
    d_pr = clone( 0, numer );
    bzero( d_pr->block_list , d_pr->size * sizeof( uint32_t ));
    memcpy( d_pr->block_list, denom->block_list, denom->size*sizeof(uint32_t));

    two.size = 1;
    two.block_list = (uint32_t*) malloc(sizeof(uint32_t));
    two.block_list[0] = 2;

    b_shift.size = 1;
    b_shift.block_list =(uint32_t*) malloc(sizeof(uint32_t));

    temp_exp = clone( 0, numer );
    bzero( temp_exp->block_list , temp_exp->size * sizeof( uint32_t ));

    // Initial up shifting
    if( msb_n == msb_d )
        goto last_divide;
    shift = msb_n - msb_d - 1;

    for ( i = 0; i < shift ; i++ ) {
        b_lshift( d_pr );
    }

    // The down shifting
    adjust = 0;
    while( b_msb( n_pr ) >= b_msb( denom ) ) {
        shift = shift - adjust;
        while( b_msb( n_pr ) > b_msb( d_pr ) ) {
            two_comp( &d_pr );
            // n_pr = n_pr - d_pr
            b_add_one( n_pr, d_pr, n_pr );
            two_comp( &d_pr );
            // quot = quot + b_exp(2, shift)
            b_shift.block_list[0] = shift;
            b_expn( &two, &b_shift, temp_exp );
            b_add_one(quot, temp_exp, quot);
            bzero( temp_exp->block_list , temp_exp->size * sizeof( uint32_t ));
        }
        if( b_msb( n_pr ) <= b_msb( denom ) )
            goto last_divide;

        adjust = b_msb( d_pr ) - b_msb( n_pr ) + 1;

        for( i = 0; i < adjust ; i++ ){
            if( b_msb( d_pr ) <= msb_d )
                goto last_divide;
            b_rshift( d_pr );
        }
    }
    goto out;

    // This would be a messy edge case in that ^ loop.
last_divide:
    // This is a hack since two_comp changes d_pr.
    memcpy(d_pr->block_list, denom->block_list, d_pr->size);
    while( b_compare( n_pr, d_pr ) <= 0 ) { // while numer > denom
            two_comp( &d_pr );
            b_add_one( n_pr, d_pr, n_pr );
            b_inc( quot );
            two_comp( &d_pr );
    }



out:
    if( b_compare( n_pr, d_pr ) == 0 ) {
        // quot += b_expr(shift)
        b_shift.block_list[0] = shift;
        b_expn( &two, &b_shift, temp_exp );
        remainder->block_list[0] = 0;
        b_inc(quot);
    } else {
        // remainder = n_pr
        memcpy(remainder->block_list, n_pr->block_list, remainder->size*sizeof(uint32_t));
    }

}

void free_bn( struct b_number* n) {
    free(n->block_list);
    free(n);
}
