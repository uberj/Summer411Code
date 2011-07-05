#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"
#define SB 7
#define MODEL_SIZE 254
// Better to over shoot. I'm using unsigned longs.

struct model {
    unsigned long* symbols;
    unsigned long total;
    unsigned long mHigh;
    unsigned long mLow;
    unsigned long mScale;

    /* shifting vars */
    unsigned long g_Half;
    unsigned long g_FirstQuarter;
    unsigned long g_ThirdQuarter;
};

/* Initialize the variables needed to do E[1-3] shifting
 */
void init_shifting_model( struct model *mdl );
/* Create model 0 model. Count the frequence of each char.
 * */
void populate_model( FILE *fp, struct model* mdl );
/* Print the array of frequencies to the screen.
 */
void print_model( struct model* mdl );
/* Using a model and a file, encode to a stream of bits.
 */
void encode_with_model( FILE *fp, struct model* mdl );
void setBit( struct model *mdl, int bit );

void do_one_encode( struct model *mdl, unsigned long low_count, unsigned long high_count, unsigned long total ) {
    unsigned long g_Half = mdl->g_Half;
    unsigned long g_ThirdQuarter = mdl->g_ThirdQuarter;
    unsigned long g_FirstQuarter = mdl->g_FirstQuarter;
    unsigned long mScale = mdl->mScale;
    unsigned long mHigh = mdl->mHigh;
    unsigned long mLow  = mdl->mLow;
    unsigned long mStep = ( mHigh - mLow + 1 ) / total;
    mHigh   = mLow + mStep*high_count - 1;
    mLow    = mLow + mStep*low_count;

    //E1-E2 Scale
    while( ( mHigh < g_Half ) || ( mLow >= g_Half ) ) {
        if( mHigh < g_Half ) { //E1
            setBit( mdl , 0 );
            mLow = mLow * 2;
            mHigh = mHigh * 2 + 1;

            // Check for E3
            for(;mScale > 0; mScale--){
                setBit( mdl , 1 );
            }
        } else if( mLow >= g_Half ) { //E2
            setBit( mdl, 1 );
            mLow = 2 * ( mLow - g_Half );
            mHigh = 2 * ( mHigh - g_Half ) + 1;

            // Check for E3
            for(;mScale > 0; mScale--){
                setBit( mdl , 0 );
            }
        }
    }

    //E3 Scale
    while( ( g_FirstQuarter <= mLow ) && ( mHigh < g_ThirdQuarter ) ) {
        mScale++;
        mLow = 2 * ( mLow - g_FirstQuarter );
        mHigh = 2 * ( mHigh - g_FirstQuarter ) + 1;
    }

    // Update model state
    mdl->g_Half = g_Half;
    mdl->g_ThirdQuarter = g_ThirdQuarter;
    mdl->g_FirstQuarter = g_FirstQuarter;
    mdl->mScale = mScale;
    mdl->mHigh = mHigh;
    mdl->mLow = mLow;

}

void encode_with_model( FILE *fp, struct model* mdl ){
    char sym;
    int i;
    unsigned long low_count;

    for( sym = fgetc(fp); sym != '\n' ; sym = fgetc(fp) ) {
        // Caluculate low count
        low_count = 0;
        for(i=97;(char)i<sym;i++){
            if( !mdl->symbols[i] )
                continue;
            else
                low_count += mdl->symbols[i];
        }
        // Highcount is the next non-zero symbol in symbols... find it.
        while(!mdl->symbols[i]){
            i++;
        }
        do_one_encode( mdl, low_count, low_count + mdl->symbols[i], mdl->total );
    }
}

void setBit( struct model *mdl, int bit ) {
    printf("%i",bit);
}

void init_shifting_model( struct model *mdl ) {
    mdl->g_Half = pow( 2, (SB-1) );
    mdl->g_FirstQuarter = mdl->g_Half/2;
    mdl->g_ThirdQuarter = mdl->g_Half + mdl->g_FirstQuarter;
    mdl->mLow = 0;
    mdl->mHigh = 0x7F;
    mdl->mScale = 0;
}

void populate_model( FILE *fp, struct model* mdl ) {
    char sym;
    unsigned long i;
    mdl->symbols = calloc( MODEL_SIZE, sizeof( unsigned long ) );
    mdl->total = 0;
    for( sym = fgetc(fp); sym != '\n' ; sym = fgetc(fp) ) {
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
