#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bs.h"


#define bs_dump(x) word_dump(x, BLOCK_SIZE)

word_t RINPUT[BLOCK_SIZE];
word_t RINPUT_RES[BLOCK_SIZE];

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
    memmove(blocks,INPUT,sizeof(blocks));
    bs_transpose(blocks);

    bs_transpose_rev(blocks);
    
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

    bs_sbox(sbox_in);
    
    printf("SBOX output:\n");
    word_dump(sbox_in,8);

    bs_sbox_rev(sbox_in);

    printf("SBOX reverse:\n");
    word_dump(sbox_in,8);
}
void test_mixcolumns()
{
    word_t mixcolumn[BLOCK_SIZE];
    word_t mixcolumn_out[BLOCK_SIZE];

    memmove(mixcolumn, INPUT[0], 8 * BLOCK_SIZE);

    printf("MIXCOLUMNS input:\n");
    word_dump(mixcolumn,BLOCK_SIZE);

    bs_mixcolumns(mixcolumn);

    printf("MIXCOLUMNS output:\n");
    word_dump(mixcolumn_out,BLOCK_SIZE);
    
    memset(mixcolumn,0,sizeof(mixcolumn));
    bs_mixcolumns_rev(mixcolumn);
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
    
    bs_shiftrows(shiftrow);
    printf("SHIFTROWS output:\n");
    word_dump(shiftrow,BLOCK_SIZE);
    
    memset(shiftrow,0,sizeof(shiftrow));
    bs_shiftrows_rev(shiftrow);
   
    printf("SHIFTROWS reverse:\n");
    word_dump(shiftrow,BLOCK_SIZE);
}

void bs_addroundkey(word_t * B, word_t * rk)
{
    int i;
    for (i = 0; i < BLOCK_SIZE; i++)
        B[i] ^= rk[i];
}

void test_addroundkey()
{
    word_t input[BLOCK_SIZE];
    word_t output[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    int i;
    for (i=0; i<11; i++)
    {
        memset(rk[i], (i+1)*253, BLOCK_SIZE);
    }
    memmove(input, INPUT, BLOCK_SIZE * WORD_SIZE / 8);

    memmove(output, input, BLOCK_SIZE * WORD_SIZE / 8);

    for (i=0; i < 10; i++)
        bs_addroundkey(output, rk[i]);

    for (i=9; i >= 0; i--)
        bs_addroundkey(output, rk[i]);
    
    printf("test round key output:\n");
    block_dump(output,WORD_SIZE);

}

void bs_apply_sbox(word_t * input)
{
    int i;
    for(i=0; i < BLOCK_SIZE; i+=8)
    {
        bs_sbox(input+i);
    }
}

void bs_apply_sbox_rev(word_t * input)
{
    int i;
    for(i=0; i < BLOCK_SIZE; i+=8)
    {
        bs_sbox_rev(input+i);
    }
}

#define check_mem(m1,m2) _check_mem(m1,m2,__LINE__)

static void _check_mem(void * m1, void * m2, int line)
{
    if (memcmp(m1,m2,BLOCK_SIZE/8)== 0)
    {
        printf("(%d) mem is same\n", line);
    }
    else
    {
        printf("(%d) mem is not the same\n",line);
    }
}

void test_all_steps()
{
    word_t input[BLOCK_SIZE];
    word_t input2[BLOCK_SIZE];
    word_t output[BLOCK_SIZE];
    word_t trans[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    int i;
    for (i=0; i<11; i++)
    {
        memset(rk[i], i, BLOCK_SIZE);
    }

    memmove(input, RINPUT, BLOCK_SIZE * WORD_SIZE / 8);
    memmove(input2, RINPUT, BLOCK_SIZE * WORD_SIZE / 8);
    memset(trans,0, BLOCK_SIZE * WORD_SIZE / 8);


    bs_transpose(input);
    bs_mixcolumns(input);
    bs_transpose_rev(input);
    MixColumns((state_t *)input2);
    check_mem(input,input2);


    bs_transpose(input);
    bs_shiftrows(input);
    bs_transpose_rev(input);
    ShiftRows((state_t *)input2);
    check_mem(input,input2);

    bs_transpose(input);
    bs_mixcolumns(input);
    bs_transpose_rev(input);
    MixColumns((state_t *)input2);
    check_mem(input,input2);


    bs_transpose(input);
    bs_shiftrows(input);
    bs_transpose_rev(input);
    ShiftRows((state_t *)input2);
    check_mem(input,input2);


    bs_transpose(input);
    bs_shiftrows_rev(input);
    bs_transpose_rev(input);
    InvShiftRows((state_t *)input2);
    check_mem(input,input2);

    bs_transpose(input);
    bs_mixcolumns_rev(input);
    bs_transpose_rev(input);
    InvMixColumns((state_t *)input2);
    check_mem(input,input2);

    bs_transpose(input);
    bs_shiftrows_rev(input);
    bs_transpose_rev(input);
    InvShiftRows((state_t *)input2);
    check_mem(input,input2);

    bs_transpose(input);
    bs_mixcolumns_rev(input);
    bs_transpose_rev(input);
    InvMixColumns((state_t *)input2);
    check_mem(input,input2);


    int wow = 0;

    for (wow = 0; wow < 10; wow++)
    {
        bs_addroundkey(input, rk[3]);
        bs_shiftrows(input);
        bs_mixcolumns(input);
        bs_shiftrows(input);
        bs_addroundkey(input, rk[3]);
        bs_apply_sbox(input);
        bs_shiftrows(input);
    }

    for (wow = 0; wow < 10; wow++)
    {
        bs_shiftrows_rev(input);
        bs_apply_sbox_rev(input);
        bs_addroundkey(input, rk[3]);
        bs_shiftrows_rev(input);
        bs_mixcolumns_rev(input);
        bs_shiftrows_rev(input);
        bs_addroundkey(input, rk[3]);
    }
    /*bs_apply_sbox_rev(input,input);*/
    /*ShiftRows((state_t *)input2);*/
    /*bs_mixcolumns_rev(input);*/
    /*block_dump(input, WORD_SIZE);*/


    if (memcmp(input,RINPUT,BLOCK_SIZE/8)== 0)
    {
        printf("mem is the same\n");
    }
    else
    {
        printf("mem is not the same\n");
    }

    /*printf("bs mem:\n");*/
    /*hex_dump((uint8_t*)input,BLOCK_SIZE/8);*/
    /*printf("ref mem:\n");*/
    /*hex_dump((uint8_t*)input2,BLOCK_SIZE/8);*/

}

void test_aes()
{
    word_t input[BLOCK_SIZE];
    word_t input2[BLOCK_SIZE];
    word_t output[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    int i;
    for (i=0; i<11; i++)
    {
        memset(rk[i], i * 19, BLOCK_SIZE);
    }

    memmove(input, RINPUT, BLOCK_SIZE * WORD_SIZE / 8);
    memmove(input2, RINPUT, BLOCK_SIZE * WORD_SIZE / 8);
    
    bs_transpose(input);

    // add round key
    bs_addroundkey(input,rk[0]);
    

    int rounds = 1;

    for (; rounds < 10; rounds++)
    {
        bs_apply_sbox(input);
        bs_shiftrows(input);
        bs_mixcolumns(input);
        bs_addroundkey(input,rk[i]);
    }

    bs_apply_sbox(input);
    bs_shiftrows(input);
    bs_addroundkey(input,rk[10]);

    // decrypt

    bs_addroundkey(input,rk[10]);
    // undo rounds
    for (rounds = 9; rounds > 0; rounds--)
    {
        bs_shiftrows_rev(input);
        bs_apply_sbox_rev(input);
        bs_addroundkey(input,rk[i]);
        bs_mixcolumns_rev(input);
    }
    bs_shiftrows_rev(input);
    bs_apply_sbox_rev(input);

    bs_addroundkey(input,rk[0]);

    bs_transpose_rev(input);

    if (memcmp(input,input2,BLOCK_SIZE*WORD_SIZE/8) == 0)
    {
        printf("AES test passed!\n");
    }
    else
    {
        printf("AES test failed!\n");
    }
}


void test_steps_nested()
{
    word_t input[BLOCK_SIZE];
    word_t input2[BLOCK_SIZE];
    word_t output[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    int i;
    for (i=0; i<11; i++)
    {
        memset(rk[i], i * 19, BLOCK_SIZE);
    }

    memmove(input, RINPUT, BLOCK_SIZE * WORD_SIZE / 8);
    memmove(input2, RINPUT, BLOCK_SIZE * WORD_SIZE / 8);

    bs_transpose(input);

    // add round key
    bs_addroundkey(input,rk[0]);
    

    int rounds = 1;

    for (; rounds < 10; rounds++)
    {
        bs_apply_sbox(input);
        bs_shiftrows(input);
        bs_mixcolumns(input);
        bs_addroundkey(input,rk[i]);
    }

    bs_apply_sbox(input);
    bs_shiftrows(input);
    bs_addroundkey(input,rk[10]);

    // decrypt

    bs_addroundkey(input,rk[10]);
    // undo rounds
    for (rounds = 9; rounds > 0; rounds--)
    {
        bs_shiftrows_rev(input);
        bs_apply_sbox_rev(input);
        bs_addroundkey(input,rk[i]);
        bs_mixcolumns_rev(input);
    }
    bs_shiftrows_rev(input);
    bs_apply_sbox_rev(input);

    bs_addroundkey(input,rk[0]);

    memmove(output,input,BLOCK_SIZE*WORD_SIZE/8);
    printf("all steps nested output:\n");
    block_dump(output,64);
}

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,"usage: %s <input-file>\n", argv[0]);
        exit(1);
    }
    FILE * r = fopen(argv[1], "r");
    if (r == NULL)
    {
        perror("fopen");
        exit(2);
    }
    int n = read(fileno(r), RINPUT, sizeof(RINPUT));
    if (n != sizeof(RINPUT))
    {
        fprintf(stderr,"file does not have enough bytes\n");
        exit(2);
    }
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
    uint8_t inputz[16] = "\xb5\x5a\xa6\xaa\xa7\x91\x20\x04\x92\xc0\xe9\x20\xa2\x69\x8b\x95";
    /*uint8_t inputz[16] = "\x05\x0a\x06\x0a\x07\x01\x00\x04\x02\x00\x09\x00\x02\x09\x0b\x05";*/
    hex_dump((uint8_t*)inputz,16);
   
    bs_transpose(input, (word_t*)inputz);

    /*printf("Transpose:\n");*/
    /*word_dump(input,BLOCK_SIZE);*/

    bs_mixcolumns(output,input);
    
    /*printf("shift rows raw:\n");*/
    /*word_dump(output,BLOCK_SIZE);*/

    memset(input,0, BLOCK_SIZE * WORD_SIZE / 8);
    bs_transpose_rev(input, output);

    printf("mixcolumns:\n");
    hex_dump((uint8_t*)input,16);
    
    memset(input,0, BLOCK_SIZE * WORD_SIZE / 8);
    bs_mixcolumns_rev(output);
    memset(output,0, BLOCK_SIZE * WORD_SIZE / 8);
    bs_transpose_rev(output, input);
    
    printf("mixcolumns reverse:\n");
    hex_dump((uint8_t*)output,16);
#else
    /*test_steps_nested();*/
    /*test_all_steps();*/
    test_aes();

    /*test_addroundkey();*/
#endif



    return 0;
}


