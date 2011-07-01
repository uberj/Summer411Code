#define _FILE_OFFSET_BITS 64
#define MAX_SEQ_SIZE 2
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "uthash.h"

struct symbol {
    int count;
    char *seq; // Sequence stored in this token.
    UT_hash_handle hh;
};

void add_symbol( char *new_seq, int seq_size, struct symbol** symbols ) {
    struct symbol* new_symbol;
    if ( (new_symbol = (struct symbol*)malloc(sizeof(struct symbol))) == NULL) exit(-1);
    new_symbol->count = 1;
    // INIT the space for the sequence
    if ( (new_symbol->seq = (char *)malloc(sizeof(char)*seq_size)) == NULL) exit(-1);
    new_symbol->seq = malloc(sizeof(char)*seq_size);
    // Copy in the new symbol. This will now be a unique key in the dictionary.
    strcpy( new_symbol->seq, new_seq );
    HASH_ADD_KEYPTR(hh,*symbols,new_symbol->seq,seq_size,new_symbol);
}
void count_inc_symbol( struct symbol *symbol ) {
    symbol->count++;
}

void print_symbols( struct symbol* symbols ){
    struct symbol* temp;
    for(temp=symbols; temp != NULL; temp=temp->hh.next) {
        printf("symbol count %d: sequence %s\n", temp->count, temp->seq);
    }
}

int main(int argc, char **argv){
    char *fname = 0;
    char omega[MAX_SEQ_SIZE];
    char new;
    FILE *file;
    int i = 0;
    int j = 0;
    int o_size = 0;
    struct symbol *symbols = NULL;
    struct symbol *temp = NULL;

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

    for( new = fgetc(file); new != EOF ; new = fgetc(file) ) {
        omega[o_size] = new;
        omega[o_size+1] = 0;
        o_size++;
        HASH_FIND( hh, symbols, omega , o_size , temp );
        if (temp) {
            temp->count++;
            temp = NULL;
        } else {
            // create space for new symbol
            if ( (temp = (struct symbol*)malloc(sizeof(struct symbol))) == NULL) exit(-1);
            temp->count = 1;
            // INIT the space for the sequence
            if ( (temp->seq = (char *)malloc(sizeof(char)*o_size)) == NULL) exit(-1);
            strcpy( temp->seq, omega );
            HASH_ADD_KEYPTR(hh,symbols,temp->seq,o_size,temp);
            // Reset omega
            omega[0] = 0;
            o_size = 0;
        }
        i++;
    }

    print_symbols( symbols );
    fclose(file);
    return 0;
}

