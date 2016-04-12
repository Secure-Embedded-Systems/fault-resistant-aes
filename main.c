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
        if ((len+1) % 8) printf("%" WPAD WFMT "\n",*h++);
        else printf("%d: %" WPAD WFMT "\n",128-len-1,*h++);

    printf("\n");
}

void block_dump(word_t * h, int len)
{
    while(len-=2 >= 0)
        printf("%" WPAD WFMT"%" WPAD WFMT  "\n",*h++,*h++);
    printf("\n");
}

extern uint8_t INPUT[WORD_SIZE/8][BLOCK_SIZE/8 + 1];
extern uint8_t INPUTZ[WORD_SIZE/8][BLOCK_SIZE/8 + 1];

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

void test_all_steps()
{
    word_t input[BLOCK_SIZE];
    word_t output[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    int i;
    for (i=0; i<11; i++)
    {
        memset(rk[i], i, BLOCK_SIZE);
    }

    memmove(input, INPUT, BLOCK_SIZE * WORD_SIZE / 8);

    // encrypt

    bs_apply_sbox(output,input);
    bs_apply_sbox_rev(input,output);
        
    bs_shiftrows(output,input);
    bs_shiftrows_rev(input,output);
    
    bs_mixcolumns(output,input);
    bs_mixcolumns_rev(input,output);

    bs_addroundkey(input,rk[5]);
    bs_addroundkey(input,rk[5]);

    memmove(output,input,BLOCK_SIZE*WORD_SIZE/8);
    printf("all steps output:\n");
    word_dump(output,BLOCK_SIZE);
}

void test_steps_nested()
{
    word_t input[BLOCK_SIZE];
    word_t output[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    int i;
    for (i=0; i<11; i++)
    {
        memset(rk[i], i*19, BLOCK_SIZE);
    }

    memmove(input, INPUT, BLOCK_SIZE * WORD_SIZE / 8);

    // nest
    bs_addroundkey(input,rk[0]);
    bs_apply_sbox(output,input);
    bs_shiftrows(input,output);
    bs_mixcolumns(output,input);
    bs_addroundkey(output,rk[5]);


    // un nest
    bs_addroundkey(output,rk[5]);
    bs_mixcolumns_rev(input,output);
    bs_shiftrows_rev(output,input);
    bs_apply_sbox_rev(input,output);
    bs_addroundkey(input,rk[0]);


    memmove(output,input,BLOCK_SIZE*WORD_SIZE/8);
    printf("all steps nested output:\n");
    word_dump(output,BLOCK_SIZE);
}


void test_aes()
{
    word_t input[BLOCK_SIZE];
    word_t output[BLOCK_SIZE];
    word_t trans[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    int i;
    for (i=1; i<11; i++)
    {
        memset(rk[i], i * 0x124, BLOCK_SIZE);
    }
    memset(rk[0], 0x55, BLOCK_SIZE);

    memmove(input, INPUT, BLOCK_SIZE * WORD_SIZE / 8);
    //bs_transpose(input, (word_t*)INPUT);
    memset(output,0, BLOCK_SIZE * WORD_SIZE / 8);
   
    printf("AES input:\n");
    word_dump(input,BLOCK_SIZE);
    memset(trans,0,sizeof(trans));

    // Encrypt
    // add key
    bs_addroundkey(input,rk[0]);

    int round = 1;
    for(; round < 10; round++)
    {
        bs_apply_sbox(output,input);
        bs_shiftrows(input,output);
        //memmove(output,input,BLOCK_SIZE * WORD_SIZE/8);
        bs_mixcolumns(output,input);
        bs_addroundkey(output,rk[i]);
        memmove(input,output,BLOCK_SIZE * WORD_SIZE/8);
    }
    bs_apply_sbox(input,output);
    bs_shiftrows(output,input);
    //memmove(input,output,BLOCK_SIZE * WORD_SIZE/8);
    bs_addroundkey(output,rk[10]);
    
    printf("AES output:\n");
    word_dump(output,BLOCK_SIZE);
    /*printf("transpose:\n");*/
    /*bs_transpose_rev(trans, output);*/
    /*block_dump(trans,BLOCK_SIZE);*/

    memset(trans,0,sizeof(trans));

    // Decrypt

    bs_addroundkey(output,rk[10]);
    round = 9;
    for(; round >= 1; round--)
    {
        bs_shiftrows_rev(input,output);
        //memmove(output,input,BLOCK_SIZE * WORD_SIZE/8);
        bs_apply_sbox_rev(output,input);
        bs_addroundkey(output,rk[i]);
        bs_mixcolumns_rev(input,output);
        memmove(output,input,BLOCK_SIZE * WORD_SIZE/8);
    }
    
    bs_shiftrows_rev(input,output);
    //memmove(output,input,BLOCK_SIZE * WORD_SIZE/8);
    bs_apply_sbox_rev(output,input);
    bs_addroundkey(output,rk[0]);

    printf("AES reverse:\n");
    word_dump(output,BLOCK_SIZE);

    /*printf("transpose:\n");*/
    /*bs_transpose_rev(trans, output);*/
    /*block_dump(trans,BLOCK_SIZE);*/
}


int main()
{
#if 0
    test_transpose();
    test_sbox();
    test_mixcolumns();
    test_shiftrows();
    test_aes();
#endif

#if 0
    word_t input[BLOCK_SIZE];
    word_t output[BLOCK_SIZE];

    memset(output,0, BLOCK_SIZE * WORD_SIZE / 8);
    memset(input,0, BLOCK_SIZE * WORD_SIZE / 8);
    printf("Input: \n");
    hex_dump((uint8_t*)INPUTZ,16);
   
    bs_transpose(input, (word_t*)INPUTZ);

    /*printf("Transpose:\n");*/
    /*word_dump(input,BLOCK_SIZE);*/

    bs_shiftrows(output,input);
    
    /*printf("shift rows raw:\n");*/
    /*word_dump(output,BLOCK_SIZE);*/

    memset(input,0, BLOCK_SIZE * WORD_SIZE / 8);
    bs_transpose_rev(input, output);

    printf("shift rows :\n");
    hex_dump((uint8_t*)input,16);
    
    memset(input,0, BLOCK_SIZE * WORD_SIZE / 8);
    bs_shiftrows_rev(input,output);
    memset(output,0, BLOCK_SIZE * WORD_SIZE / 8);
    bs_transpose_rev(output, input);
    
    printf("shift rows reverse:\n");
    hex_dump((uint8_t*)output,16);
#else
    test_steps_nested();
#endif



    return 0;
}


