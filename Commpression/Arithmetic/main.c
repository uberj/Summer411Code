#define _FILE_OFFSET_BITS 64
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "model.c"

int main( int argc, char **argv ) {
    FILE *out_file;
    FILE *in_file;
    char *fname;
    if(argc > 1){
        fname = argv[1];
        fprintf(stdout, "Filename is %s\n", fname);
    }
    in_file = fopen(fname, "r");
    out_file = fopen("compressed", "w+");
    if(!out_file || !in_file ){
        perror("Could not open file");
        return 1;
    }
    fprintf(stdout, "File (%s) were opened successfully\n", fname);

    struct model mdl;
    mdl.out_fp = out_file;
    mdl.in_fp = in_file;


    populate_model( in_file, &mdl );
    init_shifting_model( &mdl );
    fseek( in_file, 0 , SEEK_SET ); //Reset file
    encode_with_model( in_file, &mdl );


    fseek( out_file, 0 , SEEK_SET ); //Reset file
    init_decode( &mdl );
    decode_with_model( &mdl );
    //print_model(&mdl);
    return 0;

}
