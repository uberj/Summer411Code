/*
 * Jacques Uber 7/6/2011
 * References: Sable Technical Report No. 2007-5
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "model.h"
#define SB 7
#define MODEL_SIZE 254
#define BUF_SIZE 2
#define SYM_START 0
#define META_SIZE 3+5 // Multiply this by # symbols to get starting of message
// Better to over shoot. I'm using unsigned longs.
//
static unsigned int file_offset; // This keeps a count that is used to multiply BUF_SIZE to walk through files.

static unsigned char bit_buff;
static unsigned char bit_pos;
static unsigned char en_code[BUF_SIZE];
static unsigned char de_code[BUF_SIZE];
static unsigned int en_code_pos;
static unsigned int de_code_pos;
static unsigned int mBuffer;

//Encoding vars
unsigned short int mScale;
unsigned short int mHigh;
unsigned short int mLow;

struct model {
    FILE *out_fp;
    FILE *in_fp;
    int cardinality;
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
void do_one_decode( struct model *mdl, unsigned long low_count, unsigned long high_count);

void mask_out_bits( unsigned long *num ){
    unsigned long mask=1;
    // unsigned longs, on x86, are 4 bytes.
    // push 1's out SB digets.
    mask << SB;
    printf("num before mask %lu\n",*num);
    *num &= mask;
    printf("num after mask %lu\n",*num);

}

int get_bit( struct model *mdl ) {
    unsigned char clean = 0; //Use this guy to get bit values
    if ( de_code_pos >= BUF_SIZE )
        fill_bit_buff( mdl );

    switch ( bit_pos ) {
        case 0:
            if( de_code[de_code_pos] == EOF ){
                printf("reached the end\n");
                return -1;
            }
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
            goto out;
    }
    bit_pos++;
out:
    if(clean)
        return 1;
    else
        return 0;

}
void fill_bit_buff( struct model *mdl ){
    //char sym;
    int tell;
    int offset = 3+5*(mdl->cardinality)+1  +  BUF_SIZE*file_offset;
    fseek( mdl->out_fp, offset, SEEK_SET );
    tell = ftell( mdl->out_fp);
    //printf("\n");
    //printf("reading from pos %i in output\n",tell);
    bzero( &de_code, BUF_SIZE );
    fread( &de_code, sizeof(char), BUF_SIZE, mdl->out_fp );
    /*
    printf("Read in Buffer:\n");
    for(i=0;i<BUF_SIZE;i++){
        printf("%x ",de_code[i]);
    }
    printf("\n");
    */
    /*
    for( i=0, sym = fgetc(mdl->out_fp); i < BUF_SIZE && sym != EOF ; i++, sym = fgetc(mdl->out_fp) ) {
        tell = ftell( mdl->out_fp);
        printf("reading from pos %i in output\n",tell);
        de_code[i] = sym;
    }
    if( sym == EOF )
        de_code[i+1] = EOF;
    */
    de_code_pos = 0;
    bit_pos = 0;
    file_offset++;

}
void bull(){
    printf("h\n");
}

int find_char( struct model *mdl ) {
    unsigned long mHigh = mdl->mHigh;
    unsigned long mLow  = mdl->mLow;
    unsigned long total = mdl->total;

    unsigned long mStep = ( mHigh - mLow + 1 ) / total;
    return ( mBuffer - mLow ) / mStep;
}


void decode_with_model( struct model* mdl ) {
    int i = 0;
    int id_value;
    int sym;
    int low_count;
    int decoded_count;
    //unsigned long low_count;
    for( i=0; i < mdl->total ; i++){
        low_count = 0;
        id_value = find_char( mdl );
        for(sym = SYM_START ; low_count + mdl->symbols[sym] <= id_value; sym ++){
            low_count += mdl->symbols[sym];
        }
        printf("%c",(char)sym);
        decoded_count++;
        /*
        if(decoded_count == mdl->total+1)
            break;
        */
        // Update our model.
        do_one_decode( mdl, low_count, low_count + mdl->symbols[sym] );

    }
    printf("\n");
    return;
}

void do_one_decode( struct model *mdl, unsigned long low_count, unsigned long high_count) {
    unsigned long g_Half = mdl->g_Half;
    unsigned long g_ThirdQuarter = mdl->g_ThirdQuarter;
    unsigned long g_FirstQuarter = mdl->g_FirstQuarter;
    unsigned long total = mdl->total;
    unsigned long mScale = mdl->mScale;
    unsigned long mHigh = mdl->mHigh;
    unsigned long mLow  = mdl->mLow;
    unsigned long mStep = ( mHigh - mLow + 1 ) / total;
    mHigh   = mLow + mStep*high_count - 1;
    mLow    = mLow + mStep*low_count;

    //E1-E2 Scale
    while( ( mHigh < g_Half ) || ( mLow >= g_Half ) ) {
        if( mHigh < g_Half ) { //E1
            mLow = mLow * 2;
            mHigh = mHigh * 2 + 1;
            mBuffer = 2 * mBuffer + get_bit( mdl );
        } else if( mLow >= g_Half ) { //E2
            mLow = 2 * ( mLow - g_Half );
            mHigh = 2 * ( mHigh - g_Half ) + 1;
            mBuffer = 2 * ( mBuffer - g_Half ) + get_bit( mdl );
        }
    }

    //E3 Scale
    while( ( g_FirstQuarter <= mLow ) && ( mHigh < g_ThirdQuarter ) ) {
        mScale++;
        mLow = 2 * ( mLow - g_FirstQuarter );
        mHigh = 2 * ( mHigh - g_FirstQuarter ) + 1;
        mBuffer = 2 * ( mBuffer - g_FirstQuarter ) + get_bit( mdl );
    }

    // Update model state
    mdl->g_Half = g_Half;
    mdl->g_ThirdQuarter = g_ThirdQuarter;
    mdl->g_FirstQuarter = g_FirstQuarter;
    mdl->mScale = mScale;
    mdl->mHigh = mHigh;
    mdl->mLow = mLow;

}

void init_decode( struct model *mdl ) {
    int i;
    bit_buff = 0;
    bit_pos = 0;
    de_code_pos = 0;
    mdl->mHigh = 1 << SB;
    mdl->mLow = 0;
    file_offset = 0;
    fill_bit_buff( mdl );
    // Put the first SB into the buffer.
    for( i = 0; i < SB; i++ ) { // This will totally break if we have less than SB bits.
        mBuffer = ( mBuffer << 1 );
        if(get_bit( mdl )){
            mBuffer += 1;
        }

    }
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
    if( mStep == 0 ){
        bull();
        printf("3 h_c %lu\n",high_count);
        printf("3 l_c %lu\n",low_count);
        printf("3 mStep %lu\n",mStep);
        printf("3 total %lu\n",total);
        printf("3 mLow %lu\n",mLow);
        printf("3 mHigh %lu\n",mHigh);
    }

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

/*
 * This is serving as the init_encoding method.
 */
void encode_with_model( FILE *fp, struct model* mdl ){
    char sym;
    int i;
    unsigned long low_count;
    int tell;

    fseek( mdl->out_fp, 3+5*(mdl->cardinality)+1, SEEK_SET );
    tell = ftell(mdl->out_fp);
    printf("Starting to write at %i bytes.\n", tell );
    for( sym = fgetc(fp); sym != EOF ; sym = fgetc(fp) ) {
        printf("read %c\n",sym);
        // Caluculate low count
        low_count = 0;
        for(i = SYM_START ;(char)i<sym;i++){
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
    int i,tell;
    en_code[en_code_pos] = bit_buff;
    en_code_pos++;
    // Last write
    tell = ftell( mdl->out_fp);
    printf("writing final %x to pos %i in output\n",bit_buff,tell);
    fwrite( &en_code, 1, en_code_pos, mdl->out_fp );
    en_code_pos++;
    //Debugging stuff
    printf("last byte: %d\n",bit_buff);
    printf("And buffer is...\n");
    for( i=0; i<en_code_pos ; i++ ){
        printf("%x ",en_code[i]);
    }
    printf("\n");
}

/*
 * Notes: Right now set_bit writes to a file when it has an entire byte. Idealy it
 * should write to the file after en_code becomes full. This will save time by avoiding
 * uneeded i/o requests.
 */
void set_bit( struct model *mdl, int bit ) {
    int tell;
    switch ( bit_pos ) {
        case 0:
            if ( bit )
                bit_buff |= '\1';
            break;
        case 1:
            if ( bit )
                bit_buff |= '\2';
            break;
        case 2:
            if ( bit )
                bit_buff |= '\4';
            break;
        case 3:
            if ( bit )
                bit_buff |= 0x8;
            en_code[en_code_pos] = bit_buff;
            en_code_pos++;
            // We need a new byte. Eventually we should store more than one byte. For now just write to file.
            if(en_code_pos >= BUF_SIZE) {
                tell = ftell( mdl->out_fp);
                printf("writing %x to pos %i in output\n",bit_buff,tell);
                if (!fwrite( &en_code, 1, BUF_SIZE, mdl->out_fp ) ){
                    printf("CRAP");
                }
                en_code_pos = 0;
            }
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

/*
 * Do statistics about the data set. Both encoding and decoding routines
 * will need this data. Write the results to the output file so decoding can
 * use.
 * @post-condition: populate_model will put the OUTPUT file pointer where you should
 * start to writing compressed data.
 */
/*                 #symbols
 *  [Byte]  [Byte]  [Byte]  [Byte]  [4xByte]
 *  (  Unused )             ^--------------^
 *                          | first byte is the index of symbol
 *                          | Next four bytes are the frequnce in
 *                          | and unsigned long.
 */
void populate_model( FILE *fp, struct model* mdl ) {
    char sym;
    unsigned char i;
    char cardinal = 0;

    mdl->symbols = calloc( MODEL_SIZE, sizeof( unsigned long ) );
    mdl->total = 0;
    // 2 bytes for meta data (unused)
    fseek( mdl->out_fp, 2, SEEK_SET );
    // byte 2 is for the number of symbols represented. Write this value to
    // the file as the last thing done.
    fseek( mdl->out_fp, 1, SEEK_CUR );
    for( sym = fgetc(fp); sym != EOF ; sym = fgetc(fp) ) {
        mdl->symbols[(int)sym]++;
        mdl->total++;
    }
    for( i=0; i < MODEL_SIZE; i++ ){
        if( mdl->symbols[(int)i] ){
            fwrite( &i, sizeof(char), 1, mdl->out_fp );
            fwrite( &mdl->symbols[(int)i], sizeof(unsigned long), 1, mdl->out_fp );
            cardinal++;
        }
    }
    fseek( mdl->out_fp, 2, SEEK_SET );
    fwrite( &cardinal, sizeof(char), 1, mdl->out_fp );
    mdl->cardinality = cardinal;
    // Put fp where the compressor should start to write.
    fseek( mdl->out_fp, 4+4*(cardinal+ 1), SEEK_SET );
}

void read_model( struct model *mdl ){
    char sym;
    int i;
    char cardinal = 0;
    unsigned long count;
    unsigned char* model_buffer;

    mdl->symbols = calloc( MODEL_SIZE, sizeof( unsigned long ) );
    mdl->total = 0;
    // 2 bytes for meta data (unused)
    fseek( mdl->out_fp, 2, SEEK_SET );
    // byte 2 is for the number of symbols represented. Write this value to
    fread( &cardinal, sizeof(char), 1, mdl->out_fp );
    // There are cardinal many symbols. Read them all in one i/o request and create
    // the model.
    mdl->cardinality = (int)cardinal;
    model_buffer = (unsigned char *)malloc(cardinal*(sizeof(char)+sizeof(unsigned long)));
    fread( model_buffer, cardinal*(sizeof(char)+sizeof(unsigned long)), 1  \
            , mdl->out_fp );

    // the file as the last thing done.
    for( i=0; i < cardinal * 5; i=i+5 ){
        sym = model_buffer[i];
        memcpy(&count, &model_buffer[i+1], 4); // unsigned long 4 bytes
        mdl->symbols[(int)sym] = count;
        mdl->total += count;
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
