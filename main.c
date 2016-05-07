#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bs.h"
#include "aes.h"

#ifdef RUN_TESTS
#include "tests/tests.h"
#endif

#define bs_dump(x) dump_word(x, BLOCK_SIZE)
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

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
        else printf("%d: %" WPAD WFMT "\n",128-len-1,*h++);

    printf("\n");
}

void dump_block(word_t * h, int len)
{
    while(len-=2 >= 0)
        printf("%" WPAD WFMT"%" WPAD WFMT  "\n",*h++,*h++);
    printf("\n");
}

// TODO AES CTR
int main(int argc, char * argv[])
{

#ifdef RUN_TESTS
    aes_ecb_test();
    aes_ctr_test();
    return 0;
#endif

    if (argc < 2)
    {
        fprintf(stderr,"usage: %s <input-file>\n", argv[0]);
        exit(1);
    }
    return 0;
}


