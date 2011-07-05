#include <stdio.h>
#include <stdlib.h>
#include "model.h"
#define SB 7

void encode_with_model( FILE *fp, struct model* mdl ){
    char sym;
    for( sym = fgetc(fp); sym != EOF ; sym = fgetc(fp) ) {
    }
}

void init_shifting_model( struct model *mdl ) {
    *mdl.g_Half = 2**(SB-1);
    *mdl.g_FirstQuarter = *mdl.g_Half/2;
    *mdl.g_ThirdQuarter = *mdl.g_Half + *mdl.g_FirstQuarter;
}

void populate_model( FILE *fp, struct model* mdl ) {
    char sym;
    unsigned long i;
    mdl->symbols = calloc( MODEL_SIZE, sizeof( unsigned long ) );
    mdl->total = 0;
    for( sym = fgetc(fp); sym != EOF ; sym = fgetc(fp) ) {
        i = 0;
        i += sym; // Stuff the char into an int so we can index an array.
        mdl->symbols[i]++;
        mdl->total++;
    }
}

void print_model( struct model* mdl ) {
    int i;
    for( i=0; i < MODEL_SIZE; i++ ) {
        if(mdl->symbols[i])
            printf("%c: %lu\n",(char)i,mdl->symbols[i]);
    }
    printf("Total: %lu\n",mdl->total);
}
