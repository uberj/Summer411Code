#include <stdio.h>

int main() {

    long unsigned int arg1, arg2, add, sub, mul, quo, rem ;
    int carry = 0;

    printf( "Enter two integer numbers : " );
    scanf( "%lu%lu", &arg1, &arg2 );

    /* Perform Addition, Subtraction, Multiplication & Division */
    __asm__ (   "addl %%ebx, %%ecx;"
                "movl $0x0, %%eax;"
                "adc $0x0, %%eax;"
                : "=c" (add), "=a" (carry)
                : "b" (arg1) , "c" (arg2)

            );

    printf( "%lu + %lu = %lu\n", arg1, arg2, add );
    if(carry){
        printf("Carry bit was set\n");
    }

    return 0 ;
}
