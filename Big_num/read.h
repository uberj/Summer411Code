#ifndef READ
#define READ
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "big_num.h"

void convert_bn_to_file( FILE *fp, struct b_number** bn );

void print_bn( struct b_number *bn );

void convert_file_to_bn( FILE *fp , struct b_number** bn );

#endif
