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
    word_t mixcolumn[BLOCK_SIZE];
    word_t mixcolumn_out[BLOCK_SIZE];

    memmove(mixcolumn, INPUT[0], 8 * BLOCK_SIZE);

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
void test_shiftrows()
{
    word_t shiftrow[BLOCK_SIZE];
    word_t shiftrow_out[BLOCK_SIZE];

    memmove(shiftrow, INPUT[0], 8 * BLOCK_SIZE);
    printf("SHIFTROWS input:\n");
    word_dump(shiftrow,BLOCK_SIZE);
    
    bs_shiftrows(shiftrow_out,shiftrow);
    printf("SHIFTROWS output:\n");
    word_dump(shiftrow_out,BLOCK_SIZE);
    
    memset(shiftrow,0,sizeof(shiftrow));
    bs_shiftrows_rev(shiftrow,shiftrow_out);
   
    printf("SHIFTROWS reverse:\n");
    word_dump(shiftrow,BLOCK_SIZE);
}

void bs_addroundkey(word_t * B, word_t * rk)
{
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
        B[i] ^= rk[i];
}

void bs_apply_sbox(word_t * output, word_t * input)
{
    int i;
    for(i=0; i < BLOCK_SIZE; i+=8)
    {
        bs_sbox(output+i, input+i);
    }
}

void bs_apply_sbox_rev(word_t * output, word_t * input)
{
    int i;
    for(i=0; i < BLOCK_SIZE; i+=8)
    {
        bs_sbox_rev(output+i, input+i);
    }
}

void test_aes()
{
    word_t input[BLOCK_SIZE];
    word_t output[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    int i;
    for (i=0; i<11; i++)
    {
        memset(rk[11], i, BLOCK_SIZE);
    }

    // encrypt

    int round = 0;
    for(; round < 9; round++)
    {
        bs_apply_sbox(output,input);
        bs_shiftrows(input,output);
        bs_mixcolumns(output,input);
        bs_addroundkey(output,rk[i]);
        memmove(input,output,BLOCK_SIZE * WORD_SIZE);
    }
    bs_apply_sbox(input,output);
    bs_shiftrows(output,input);
    bs_addroundkey(output,rk[9]);

    // decrypt

    bs_addroundkey(output,rk[9]);
    round = 8;
    for(; round > 0; round--)
    {
        bs_shiftrows_rev(input,output);
        bs_apply_sbox_rev(output,input);
        bs_addroundkey(output,rk[i]);
        bs_mixcolumns(input,output);
        memmove(output,input,BLOCK_SIZE * WORD_SIZE);
    }
    
    bs_shiftrows_rev(input,output);
    bs_apply_sbox_rev(output,input);
    bs_addroundkey(output,rk[0]);

    printf("AES output:\n");
    word_dump(output,BLOCK_SIZE);

}


int main()
{
#if 0
    test_transpose();
    test_sbox();
    test_mixcolumns();
    test_shiftrows();
#endif

    test_aes();

    return 0;
}


