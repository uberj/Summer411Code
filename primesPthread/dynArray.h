/* 	dynArray.h : Dynamic Array implementation. */

struct DynArr
{
	int lock;
	int *data;		/* pointer to the data array */
	int size;		/* Number of elements in the array */
	int capacity;	/* capacity ofthe array */
};
typedef struct DynArr DynArr;


/* Dynamic Array Functions */
void initDynArr(DynArr *v, int capacity);	
DynArr *newDynArr(int cap);

void freeDynArr(DynArr *v);
void deleteDynArr(DynArr *v);

void _dynArrSetCapacity(DynArr *v, int newCap);
int sizeDynArr(DynArr *v);

void addDynArr(DynArr *v, int val);
int getDynArr(DynArr *v, int pos);
void putDynArr(DynArr *v, int pos, int val);
void swapDynArr(DynArr *v, int i, int  j);
void removeAtDynArr(DynArr *v, int idx);
int isEmptyDynArr(DynArr *v);

void copyDynArr(DynArr *source, DynArr *destination);

