
#ifndef TEST_FOOTPRINT
#include <stdio.h>
#include "utils.h"


void dump_hex(uint8_t * h, int len)
{
    while(len--)
        printf("%02hhx",*h++);
    printf("\n");
}

void dump_word(word_t * h, int len)
{
    while(len--)
        if ((len+1) % 8) printf("%" WPAD WFMT "\n",*h++);
        else printf("%d:\n%" WPAD WFMT "\n",128-len-1,*h++);

    printf("\n");
}

void dump_block(word_t * h, int len)
{
    int i = 0;
    while(len-=2 >= 0)
    {
        word_t u1 = *h++;
        word_t u2 = *h++;
        printf("%02d: %lx: %" WPAD WFMT"%" WPAD WFMT  "\n",i++,(uint64_t)h,u1,u2);
    }
    printf("\n");
    fflush(stdout);
}

#endif
