#ifndef NUM
#define NUM

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define B_SIZE 4;

struct b_number {
    uint32_t size;
    uint32_t* block_list;
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

struct b_number* b_add( struct b_number* n1, struct b_number* n2 );
void b_add_one( struct b_number* small_num, struct b_number* large_num, struct b_number* sum);
int c_add( uint32_t* arg1, uint32_t* arg2, uint32_t* sum );
struct b_number* clone( int copy, struct b_number* orig );
void bn_copy( struct b_number* n1, struct b_number* n2 ) ;
void two_comp( struct b_number** n );
void b_mult( struct b_number *mult, struct b_number *n1 );
int b_compare( struct b_number *n1, struct b_number *n2 );
void b_inc( struct b_number *n );
void b_dec( struct b_number *n );
void b_rshift( struct b_number *n );
void b_lshift( struct b_number *n );
void b_expn( struct b_number *base, struct b_number *exp, struct b_number *result ) ;
void _b_expn_test ( struct b_number *base, struct b_number *exp, struct b_number **result) ;
void b_divide(  struct b_number* numer, struct b_number* denom, \
                struct b_number** quot, struct b_number** remainder ) ;
void b_divide_one(  struct b_number* numer, struct b_number* denom, \
                struct b_number* quot, struct b_number* remainder ) ;
void sb_check( struct b_number *n, uint32_t *block, uint32_t *bit );
uint32_t b_msb( struct b_number* n) ;
void b_fast_div(  struct b_number* numer, struct b_number* denom, \
                struct b_number** quot, struct b_number** remainder ) ;
void b_fast_div_one(  struct b_number* numer, struct b_number* denom, \
                struct b_number* quot, struct b_number* remainder ) ;
#endif
