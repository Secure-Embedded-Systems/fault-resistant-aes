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

void test_mixcolumn(word_t * Bp, word_t * B)
{
    int i = 0;
    for (; i < BLOCK_SIZE / 4; i += 8)
    {
    //  of0 = (B[0] & 0x80);
    //  of5 = (B[5] & 0x80);
    //  of = B[0] ^ [5];
    //  Bp = Bp ^ (0x1b & ((signed char)of>>7));

    //// 2 * A0
    //  Bp = Bp ^ (B[0] << 1)

    //// + 3 * A1
    //  Bp = Bp ^ (B[5])
    //  Bp = Bp ^ (B[5]<<1)

    //// + A2 + A3
    //  Bp = Bp ^ (B[10])
    //  Bp = Bp ^ (B[15])
    word_t of = B[7] ^ B[47];

    Bp[0] =                B[40] ^ B[80] ^ B[120] ^ of;
    Bp[1] = B[0] ^ B[40] ^ B[41] ^ B[81] ^ B[121] ^ of;
    Bp[2] = B[1] ^ B[41] ^ B[42] ^ B[82] ^ B[122];
    Bp[3] = B[2] ^ B[42] ^ B[43] ^ B[83] ^ B[123] ^ of;
    Bp[4] = B[3] ^ B[43] ^ B[44] ^ B[84] ^ B[124] ^ of;
    Bp[5] = B[4] ^ B[44] ^ B[45] ^ B[85] ^ B[125];
    Bp[6] = B[5] ^ B[45] ^ B[46] ^ B[86] ^ B[126];
    Bp[7] = B[6] ^ B[46] ^ B[47] ^ B[87] ^ B[127];

    //  of0 = (B[5] & 0x80);
    //  of5 = (B[10] & 0x80);
    //  of = B[5] ^ B[10]
    //  Bp = Bp ^ (0x1b & ((signed char)of>>7));

    //// A0
    //  Bp = Bp ^ (B[0])

    //// + 2 * A1
    //  Bp = Bp ^ (B[5] << 1)

    //// + 3 * A2
    //  Bp = Bp ^ (B[10])
    //  Bp = Bp ^ (B[10]<<1)

    //// + A3
    //  Bp = Bp ^ (B[15])

    of = B[47] ^ B[87];

    Bp[40] = B[0]                 ^ B[80] ^ B[120] ^ of;
    Bp[41] = B[1] ^ B[40] ^ B[80] ^ B[81] ^ B[121] ^ of;
    Bp[42] = B[2] ^ B[41] ^ B[81] ^ B[82] ^ B[122];
    Bp[43] = B[3] ^ B[42] ^ B[82] ^ B[83] ^ B[123] ^ of;
    Bp[44] = B[4] ^ B[43] ^ B[83] ^ B[84] ^ B[124] ^ of;
    Bp[45] = B[5] ^ B[44] ^ B[84] ^ B[85] ^ B[125];
    Bp[46] = B[6] ^ B[45] ^ B[85] ^ B[86] ^ B[126];
    Bp[47] = B[7] ^ B[46] ^ B[86] ^ B[87] ^ B[127];

    //  of0 = (B[10] & 0x80);
    //  of5 = (B[15] & 0x80);
    //  of = B[10] ^ B[15]
    //  Bp = Bp ^ (0x1b & ((signed char)of>>7));

    //// A0 + A1
    //  Bp = Bp ^ (B[0])
    //  Bp = Bp ^ (B[5])

    //// + 2 * A2
    //  Bp = Bp ^ (B[10] << 1)

    //// + 3 * A3
    //  Bp = Bp ^ (B[15])
    //  Bp = Bp ^ (B[15]<<1)


    of = B[87] ^ B[127];

    Bp[80] = B[0] ^ B[40]                  ^ B[120] ^ of;
    Bp[81] = B[1] ^ B[41] ^ B[80] ^ B[120] ^ B[121] ^ of;
    Bp[82] = B[2] ^ B[42] ^ B[81] ^ B[121] ^ B[122];
    Bp[83] = B[3] ^ B[43] ^ B[82] ^ B[122] ^ B[123] ^ of;
    Bp[84] = B[4] ^ B[44] ^ B[83] ^ B[123] ^ B[124] ^ of;
    Bp[85] = B[5] ^ B[45] ^ B[84] ^ B[124] ^ B[125];
    Bp[86] = B[6] ^ B[46] ^ B[85] ^ B[125] ^ B[126];
    Bp[87] = B[7] ^ B[47] ^ B[86] ^ B[126] ^ B[127];

    //  of0 = (B[0] & 0x80);
    //  of5 = (B[15] & 0x80);
    //  of = B[0] ^ B[15]
    //  Bp = Bp ^ (0x1b & ((signed char)of>>7));

    //// 3 * A0
    //  Bp = Bp ^ (B[0])
    //  Bp = Bp ^ (B[0] << 1)

    //// + A1 + A2
    //  Bp = Bp ^ B[5]
    //  Bp = Bp ^ B[10]

    //// + 2 * A3
    //  Bp = Bp ^ (B[15]<<1)


    of = B[0] ^ B[127];

    Bp[120] = B[0] ^        B[40] ^ B[80]          ^ of;
    Bp[121] = B[1] ^ B[0] ^ B[41] ^ B[81] ^ B[120] ^ of;
    Bp[122] = B[2] ^ B[1] ^ B[42] ^ B[82] ^ B[121];
    Bp[123] = B[3] ^ B[2] ^ B[43] ^ B[83] ^ B[122] ^ of;
    Bp[124] = B[4] ^ B[3] ^ B[44] ^ B[84] ^ B[123] ^ of;
    Bp[125] = B[5] ^ B[4] ^ B[45] ^ B[85] ^ B[124];
    Bp[126] = B[6] ^ B[5] ^ B[46] ^ B[86] ^ B[125];
    Bp[127] = B[7] ^ B[6] ^ B[47] ^ B[87] ^ B[126];
    }


}

void test_mixcolumn_rev(word_t * Bp, word_t * B)
{
    B[7] = B[6] ^ B[46] ^ B[47] ^ B[87] ^ B[127];
    word_t of = B[7] ^ B[47];
    B[6] = B[5] ^ B[45] ^ B[46] ^ B[86] ^ B[126];
    B[5] = B[4] ^ B[44] ^ B[45] ^ B[85] ^ B[125];
    B[4] = B[3] ^ B[43] ^ B[44] ^ B[84] ^ B[124];
    B[3] = B[2] ^ B[42] ^ B[43] ^ B[83] ^ B[123];
    B[2] = B[1] ^ B[41] ^ B[42] ^ B[82] ^ B[122];
    B[1] = B[0] ^ B[40] ^ B[41] ^ B[81] ^ B[121];
    B[0] =                B[40] ^ B[80] ^ B[120];
}

int main()
{
#if 0
    test_transpose();
    test_sbox();
#endif
    word_t mixcolumn[8 * BLOCK_SIZE];
    word_t mixcolumn_out[8 * BLOCK_SIZE];
    word_t kr[8 * WORD_SIZE/8];

    memmove(mixcolumn, INPUT[0], 8 * BLOCK_SIZE);
    memmove(kr, INPUT[1], 8 * WORD_SIZE/8);

    printf("before:\n");
    word_dump(mixcolumn,BLOCK_SIZE);

    test_mixcolumn(mixcolumn_out,mixcolumn);

    printf("after:\n");
    word_dump(mixcolumn_out,BLOCK_SIZE);
    
    test_mixcolumn_rev(mixcolumn,mixcolumn_out);
    printf("reverse:\n");
    word_dump(mixcolumn,BLOCK_SIZE);

    return 0;
}


