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

extern uint8_t INPUT[word_t_SIZE/8][BLOCK_SIZE/8 + 1];

void test_transpose()
{
    word_t blocks[ BLOCK_SIZE ];
    word_t blocks_tmp[ BLOCK_SIZE ];
    memset(blocks,0, sizeof(blocks));
    memset(blocks_tmp,0, sizeof(blocks));
    bs_transpose(blocks,(word_t*)INPUT);
    bs_transpose_rev(blocks_tmp,blocks);
    
    printf("original:\n");
    bs_dump((word_t*)INPUT);

    printf("transpose:\n");
    bs_dump(blocks);

    printf("reverse transpose:\n");
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

void test_shiftrow()
{
    word_t * B = (word_t * ) INPUT;
    word_t * kr = (word_t * ) INPUT;
    

    word_t of = B[7] ^ B[47];

    B[0] =                B[40] ^ B[80] ^ B[120] ^ kr[0] ^ of;
    B[1] = B[0] ^ B[40] ^ B[41] ^ B[81] ^ B[121] ^ kr[1] ^ of;
    B[2] = B[1] ^ B[41] ^ B[42] ^ B[82] ^ B[122] ^ kr[2];
    B[3] = B[2] ^ B[42] ^ B[43] ^ B[83] ^ B[123] ^ kr[3] ^ of;
    B[4] = B[3] ^ B[43] ^ B[44] ^ B[84] ^ B[124] ^ kr[4] ^ of;
    B[5] = B[4] ^ B[44] ^ B[45] ^ B[85] ^ B[125] ^ kr[5];
    B[6] = B[5] ^ B[45] ^ B[46] ^ B[86] ^ B[126] ^ kr[6];
    B[7] = B[6] ^ B[46] ^ B[47] ^ B[87] ^ B[127] ^ kr[7];

}

int main()
{
#if 0
    test_transpose();
    test_sbox();
#endif
    test_shiftrow();

    return 0;
}


