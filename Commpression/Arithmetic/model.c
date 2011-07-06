/*
 * Jacques Uber
 * References: Sable Technical Report No. 2007-5
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"
#define SB 7
#define MODEL_SIZE 254
#define BUF_SIZE 32
// Better to over shoot. I'm using unsigned longs.

static unsigned char bit_buff;
static unsigned char bit_pos;
static unsigned char en_code[BUF_SIZE];
static unsigned char de_code[BUF_SIZE];
static unsigned int en_code_pos;
static unsigned int de_code_pos;

struct model {
    FILE *out_fp;
    FILE *in_fp;
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
void set_bit( struct model *mdl, int bit );
void flush_bit_buff( struct model *mdl);
void decode_with_model( struct model* mdl );
void fill_bit_buff( struct model *mdl );

int get_bit( struct model *mdl ) {
    unsigned char clean = 0; //Use this guy to get bit values
    if ( de_code_pos >= 32)
        fill_bit_buff( mdl );

    switch ( bit_pos ) {
        case 0:
            clean = de_code[de_code_pos] & '\1';
            break;
        case 1:
            clean = de_code[de_code_pos] & '\2';
            break;
        case 2:
            clean = de_code[de_code_pos] & '\4';
            break;
        case 3:
            clean = de_code[de_code_pos] & 0x8;
            de_code_pos++;
            bit_pos = 0;
            return clean; //Get out!
    }
    bit_pos++;
    return clean;

}
void fill_bit_buff( struct model *mdl ){
    int i;
    char sym;
    for( i=0, sym = fgetc(mdl->out_fp); i < BUF_SIZE && sym != EOF ; i++, sym = fgetc(mdl->out_fp) ) {
        de_code[i] = sym;
    }
    de_code_pos = 0;
    bit_pos = 0;

}


void decode_with_model( struct model* mdl ) {
    int i = 0;
    //unsigned long low_count;
    for( i=0; i<20; i++){
        if( get_bit( mdl ) )
            printf("1");
        else
            printf("0");
    }
    //encode_finish( mdl );
    printf("\n");
}

void init_decode( struct model *mdl ) {
    bit_buff = 0;
    bit_pos = 0;
    de_code_pos = 0;
    fill_bit_buff( mdl );
}

void encode_finish( struct model *mdl ) {
    unsigned long g_FirstQuarter = mdl->g_FirstQuarter;
    unsigned long mScale = mdl->mScale;
    unsigned long mLow  = mdl->mLow;
    int i;

    /* We know after the last encoding the bounds contain 1 quarter. Use lower bounds
     * do determine what the final bits should be.
     * Case 1: low < firstQuarter < Half <= high
     * Case 2: low < Half < thirdQuarter <= high
     */
    if( mLow < g_FirstQuarter ) {
        set_bit( mdl , 0 );
        for( i=0; i < mScale; i++) {
            set_bit( mdl, 1 );
        }
    } else {
        set_bit( mdl , 1 ); //Decoder add's zero's
    }
    flush_bit_buff( mdl );

    //Debugging - print stuff (This will eventually need to be written to a file for
    //the decoder to use)
    printf("mHigh: %lu\n", mdl->mHigh );
    printf("mLow: %lu\n", mdl->mLow );

}


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
            set_bit( mdl , 0 );
            mLow = mLow * 2;
            mHigh = mHigh * 2 + 1;

            // Check for E3
            for(;mScale > 0; mScale--){
                set_bit( mdl , 1 );
            }
        } else if( mLow >= g_Half ) { //E2
            set_bit( mdl, 1 );
            mLow = 2 * ( mLow - g_Half );
            mHigh = 2 * ( mHigh - g_Half ) + 1;

            // Check for E3
            for(;mScale > 0; mScale--){
                set_bit( mdl , 0 );
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
    encode_finish( mdl );
}

void flush_bit_buff( struct model *mdl ) {
    int i;
    en_code[en_code_pos] = bit_buff;
    // Last write
    fwrite( &bit_buff, 1, 1, mdl->out_fp );
    en_code_pos++;
    //Debugging stuff
    printf("last byte: %d\n",bit_buff);
    printf("And buffer is...\n");
    for( i=0; i<en_code_pos ; i++ ){
        printf("%d ",en_code[i]);
    }
    printf("\n");
}

/*
 * Notes: Right now set_bit writes to a file when it has an entire byte. Idealy it
 * should write to the file after en_code becomes full. This will save time by avoiding
 * uneeded i/o requests.
 */
void set_bit( struct model *mdl, int bit ) {
    switch ( bit_pos ) {
        case 0:
            if ( bit ){
                bit_buff |= '\1';printf("0");
            } else {
                printf("X");
            }
            break;
        case 1:
            if ( bit ){
                bit_buff |= '\2';printf("1");
            }else {
                printf("X");
            }

            break;
        case 2:
            if ( bit ){
                bit_buff |= '\4';printf("2");
            }else {
                printf("X");
            }

            break;
        case 3:
            if ( bit ){
                bit_buff |= 0x8;printf("3");
            }else {
                printf("X");
            }

            // We need a new byte. Eventually we should store more than one byte. For now just write to file.
            printf("new byte: %d\n",bit_buff);
            fwrite( &bit_buff, 1, 1, mdl->out_fp );
            en_code[en_code_pos] = bit_buff;
            en_code_pos++;
            bit_buff = 0x0;
            bit_pos = 0;
            return; //Get out!
    }
    bit_pos++;
}

void init_shifting_model( struct model *mdl ) {
    mdl->g_Half = pow( 2, (SB-1) );
    mdl->g_FirstQuarter = mdl->g_Half/2;
    mdl->g_ThirdQuarter = mdl->g_Half + mdl->g_FirstQuarter;
    mdl->mLow = 0;
    mdl->mHigh = 0x7F;
    mdl->mScale = 0;
    bit_pos = 0;
    bit_buff = 0;
    en_code_pos = 0;
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
