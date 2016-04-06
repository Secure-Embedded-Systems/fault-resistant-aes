#include <stdio.h>
#include <string.h>
#include "bs.h"


#define bs_dump(x) word_dump(x, BLOCK_SIZE)

void hex_dump(uint8_t * h, int len)
{
    while(len--)
        printf("%02hhx",*h++);
    printf("\n");
}

void word_dump(word_t * h, int len)
{
    while(len--)
        printf("%" WPAD WFMT "\n",*h++);
    printf("\n");
}

extern uint8_t INPUT[WORD_SIZE/8][BLOCK_SIZE/8 + 1];

void test_transpose()
{
    word_t blocks[ BLOCK_SIZE ];
    word_t blocks_tmp[ BLOCK_SIZE ];
    memset(blocks,0, sizeof(blocks));
    memset(blocks_tmp,0, sizeof(blocks));
    bs_transpose(blocks,(word_t*)INPUT);
    bs_transpose_rev(blocks_tmp,blocks);
    
    printf("TRANSPOSE original:\n");
    bs_dump((word_t*)INPUT);

    printf("TRANSPOSE output:\n");
    bs_dump(blocks);

    printf("TRANSPOSE reverse:\n");
    bs_dump(blocks_tmp);
}

void test_sbox()
{
    word_t * sbox_in = (word_t * ) INPUT;
    word_t sbox_out[8];
    word_t sbox_rev[8];
    int idx = 7;

    printf("SBOX input:\n");
    word_dump(sbox_in,8);

    bs_sbox(sbox_out, sbox_in);
    
    printf("SBOX output:\n");
    word_dump(sbox_out,8);

    bs_sbox_rev(sbox_rev, sbox_out);

    printf("SBOX reverse:\n");
    word_dump(sbox_rev,8);
}
void test_mixcolumns()
{
    word_t mixcolumn[8 * BLOCK_SIZE];
    word_t mixcolumn_out[8 * BLOCK_SIZE];
    word_t kr[8 * WORD_SIZE/8];

    memmove(mixcolumn, INPUT[0], 8 * BLOCK_SIZE);
    memmove(kr, INPUT[1], 8 * WORD_SIZE/8);

    printf("MIXCOLUMNS input:\n");
    word_dump(mixcolumn,BLOCK_SIZE);

    bs_mixcolumns(mixcolumn_out,mixcolumn);

    printf("MIXCOLUMNS output:\n");
    word_dump(mixcolumn_out,BLOCK_SIZE);
    
    memset(mixcolumn,0,sizeof(mixcolumn));
    bs_mixcolumns_rev(mixcolumn,mixcolumn_out);
    printf("MIXCOLUMNS reverse:\n");
    word_dump(mixcolumn,BLOCK_SIZE);
}



int main()
{
#if 0
    test_transpose();
    test_sbox();
#endif
    
    test_mixcolumns();

    return 0;
}


