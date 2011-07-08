#define _FILE_OFFSET_BITS 64
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "model.c"

int main( int argc, char **argv ) {
    FILE *out_file;
    FILE *in_file;
    char *fname;
    char *o_fname;
    struct model mdl;
    init_shifting_model( &mdl );

    if(argc > 1){
        fname = argv[2];
        fprintf(stdout, "Filename is %s\n", fname);
    }
    if( !strcmp(argv[1],"-c") ){
        printf("Compressing...\n");

        /* Create name for new compressed file */
        o_fname = (char *)malloc(strlen(fname)+4); // 4 extra?
        strcpy( o_fname, fname );
        strcpy( o_fname+strlen(fname), ".acp" );
        printf( "Writing to file %s\n", o_fname );


        in_file = fopen(fname, "r");
        out_file = fopen( o_fname, "w+");
        if(!out_file || !in_file ){
            perror("Could not open file");
            return 1;
        }

        mdl.out_fp = out_file;
        mdl.in_fp = in_file;
        populate_model( in_file, &mdl ); // File pointer is set correctly by this function.
        //print_model( &mdl );
        /*
        printf("Model before\n");
        fseek( out_file, 0 , SEEK_SET ); //Reset file
        read_model( &mdl );
        printf("Model after\n");
        print_model( &mdl );
        */
        fseek( in_file, 0 , SEEK_SET ); //Reset file
        encode_with_model( in_file, &mdl );

        printf("Done compressing.\n");
        fclose(out_file);
        fclose(in_file);

    } else if ( !strcmp(argv[1],"-x") ){
        fprintf(stdout, "Extracting from file %s...", fname);
        out_file = fopen( fname, "r");
        if(!out_file){
            perror("Could not open file");
            return 1;
        }
        //Eventuall we need to write to a file instead of stdout.
        fseek( out_file, 0 , SEEK_SET ); //Reset file
        mdl.out_fp = out_file;
        //mdl.in_fp = in_file;

        read_model( &mdl );
        init_decode( &mdl );
        decode_with_model( &mdl );
    }





    //print_model(&mdl);
    return 0;

}
