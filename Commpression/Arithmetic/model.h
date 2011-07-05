#define MODEL_SIZE 254

struct model {
    unsigned long* symbols;
    unsigned char mStep;
    unsigned char mHigh;
    unsigned char mLow;

    /* shifting vars */
    unsigned char g_Half;
    unsigned char g_FirstQuarter;
    unsigned char g_ThirdQuarter;
};

/* Initialize the variables needed to do E[1-3] shifting
 */
void init_shifting_model( struct model *mdl ) {
/* Create model 0 model. Count the frequence of each char.
 * */
void populate_model( FILE *fp, struct model* mdl );
/* Print the array of frequencies to the screen.
 */
void print_model( struct model* mdl );
/* Using a model and a file, encode to a stream of bits.
 */
void encode_with_model( FILE *fp, struct model* mdl );
