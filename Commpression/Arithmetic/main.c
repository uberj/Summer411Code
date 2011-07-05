#define _FILE_OFFSET_BITS 64
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "model.h"

int main( int argc, char **argv ) {
    FILE *file;
    char *fname;
    if(argc > 1){
        fname = argv[1];
        fprintf(stdout, "Filename is %s\n", fname);
    }
    file = fopen(fname, "r");
    if(!file){
        perror("Could not open file");
        return 1;
    }
    fprintf(stdout, "File %s was open success fully\n", fname);
    
    struct model mdl;
    populate_model( file, &mdl );
    init_shifting_model( &mdl );
    encode_with_model( file, &mdl );
    print_model(&mdl);
    return 0;

}
