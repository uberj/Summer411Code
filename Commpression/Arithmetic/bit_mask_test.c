#define _FILE_OFFSET_BITS 64
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "model.c"

int main( int argc, char **argv ) {
    unsigned long test;
    test = 782;
    mask_out_bits(&test);
    return 0;

}
