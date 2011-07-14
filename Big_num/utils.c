

struct b_number* clone( int copy, struct b_number* orig ){
    struct b_number* new;
    new = (struct b_number*) calloc( 1, sizeof(struct b_number)); 
    new->block_list = (unsigned long int*) malloc(sizeof(unsigned long int) * orig->size );
    new->size = orig->size;
    if( copy )
        memcpy( new->block_list, orig->block_list, sizeof(unsigned long int) * orig->size );
    return new;
}
