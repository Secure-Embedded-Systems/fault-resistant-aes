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

#define A0  0
#define A1  8
#define A2  16
#define A3  24

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
    //          A0.7    A1.7
    word_t of = B[A0+7] ^ B[A1+7];

    //          2*A0     2*A1          A1      A2           A3
    Bp[A0+0] =                     B[A1+0] ^ B[A2+0] ^ B[A3+0] ^ of;
    Bp[A0+1] = B[A0+0] ^ B[A1+0] ^ B[A1+1] ^ B[A2+1] ^ B[A3+1] ^ of;
    Bp[A0+2] = B[A0+1] ^ B[A1+1] ^ B[A1+2] ^ B[A2+2] ^ B[A3+2];
    Bp[A0+3] = B[A0+2] ^ B[A1+2] ^ B[A1+3] ^ B[A2+3] ^ B[A3+3] ^ of;
    Bp[A0+4] = B[A0+3] ^ B[A1+3] ^ B[A1+4] ^ B[A2+4] ^ B[A3+4] ^ of;
    Bp[A0+5] = B[A0+4] ^ B[A1+4] ^ B[A1+5] ^ B[A2+5] ^ B[A3+5];
    Bp[A0+6] = B[A0+5] ^ B[A1+5] ^ B[A1+6] ^ B[A2+6] ^ B[A3+6];
    Bp[A0+7] = B[A0+6] ^ B[A1+6] ^ B[A1+7] ^ B[A2+7] ^ B[A3+7];

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

    /*
    of = B[A1+7] ^ B[A2+7];

    //          A0      2*A1        2*A2      A2        A3
    Bp[A1+0] = B[A0+0]                     ^ B[A2+0] ^ B[A3+0] ^ of;
    Bp[A1+1] = B[A0+1] ^ B[A1+0] ^ B[A2+0] ^ B[A2+1] ^ B[A3+1] ^ of;
    Bp[A1+2] = B[A0+2] ^ B[A1+1] ^ B[A2+1] ^ B[A2+2] ^ B[A3+2];
    Bp[A1+3] = B[A0+3] ^ B[A1+2] ^ B[A2+2] ^ B[A2+3] ^ B[A3+3] ^ of;
    Bp[A1+4] = B[A0+4] ^ B[A1+3] ^ B[A2+3] ^ B[A2+4] ^ B[A3+4] ^ of;
    Bp[A1+5] = B[A0+5] ^ B[A1+4] ^ B[A2+4] ^ B[A2+5] ^ B[A3+5];
    Bp[A1+6] = B[A0+6] ^ B[A1+5] ^ B[A2+5] ^ B[A2+6] ^ B[A3+6];
    Bp[A1+7] = B[A0+7] ^ B[A1+6] ^ B[A2+6] ^ B[A2+7] ^ B[A3+7];
    */

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


    /*
    of = B[A2+7] ^ B[A3+7];

    //          A0      A1          2*A2       2*A3         A3
    Bp[A2+0] = B[A0+0] ^ B[A1+0]                     ^ B[A3+0] ^ of;
    Bp[A2+1] = B[A0+1] ^ B[A1+1] ^ B[A2+0] ^ B[A3+0] ^ B[A3+1] ^ of;
    Bp[A2+2] = B[A0+2] ^ B[A1+2] ^ B[A2+1] ^ B[A3+1] ^ B[A3+2];
    Bp[A2+3] = B[A0+3] ^ B[A1+3] ^ B[A2+2] ^ B[A3+2] ^ B[A3+3] ^ of;
    Bp[A2+4] = B[A0+4] ^ B[A1+4] ^ B[A2+3] ^ B[A3+3] ^ B[A3+4] ^ of;
    Bp[A2+5] = B[A0+5] ^ B[A1+5] ^ B[A2+4] ^ B[A3+4] ^ B[A3+5];
    Bp[A2+6] = B[A0+6] ^ B[A1+6] ^ B[A2+5] ^ B[A3+5] ^ B[A3+6];
    Bp[A2+7] = B[A0+7] ^ B[A1+7] ^ B[A2+6] ^ B[A3+6] ^ B[A3+7];
    */

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

    /*
    of = B[A0+7] ^ B[A3+7];

    //        2*A0       A0         A1         A2       2*A3
    Bp[A3+0] = B[A0+0] ^           B[A1+0] ^ B[A2+0]           ^ of;
    Bp[A3+1] = B[A0+1] ^ B[A0+0] ^ B[A1+1] ^ B[A2+1] ^ B[A3+0] ^ of;
    Bp[A3+2] = B[A0+2] ^ B[A0+1] ^ B[A1+2] ^ B[A2+2] ^ B[A3+1];
    Bp[A3+3] = B[A0+3] ^ B[A0+2] ^ B[A1+3] ^ B[A2+3] ^ B[A3+2] ^ of;
    Bp[A3+4] = B[A0+4] ^ B[A0+3] ^ B[A1+4] ^ B[A2+4] ^ B[A3+3] ^ of;
    Bp[A3+5] = B[A0+5] ^ B[A0+4] ^ B[A1+5] ^ B[A2+5] ^ B[A3+4];
    Bp[A3+6] = B[A0+6] ^ B[A0+5] ^ B[A1+6] ^ B[A2+6] ^ B[A3+5];
    Bp[A3+7] = B[A0+7] ^ B[A0+6] ^ B[A1+7] ^ B[A2+7] ^ B[A3+6];
    */
    }


}

void test_mixcolumn_rev(word_t * Bp, word_t * B)
{

    /*
        (*state)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
        (*state)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
        (*state)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
        (*state)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
    */

    word_t of0 = (B[A0+7] ^ B[A1+7]);
    word_t of1 = (B[A0+6] ^ B[A1+6]);
    word_t of2 = (B[A0+5] ^ B[A1+5]);
    word_t of3 = (B[A0+4] ^ B[A1+4]);

    word_t of = B[A0+7] ^ B[A1+7];

    //          2*A0     2*A1          A1      A2           A3
    Bp[A0+0] =                     B[A1+0] ^ B[A2+0] ^ B[A3+0] ^ of0;
    Bp[A0+1] = B[A0+0] ^ B[A1+0] ^ B[A1+1] ^ B[A2+1] ^ B[A3+1] ^ of0 ^ of1;
    Bp[A0+2] = B[A0+1] ^ B[A1+1] ^ B[A1+2] ^ B[A2+2] ^ B[A3+2];      ^ of1;
    Bp[A0+3] = B[A0+2] ^ B[A1+2] ^ B[A1+3] ^ B[A2+3] ^ B[A3+3] ^ of0       ^ of2;
    Bp[A0+4] = B[A0+3] ^ B[A1+3] ^ B[A1+4] ^ B[A2+4] ^ B[A3+4] ^ of0 ^ of1 ^ of2;
    Bp[A0+5] = B[A0+4] ^ B[A1+4] ^ B[A1+5] ^ B[A2+5] ^ B[A3+5];      ^ of1 ^ of2;
    Bp[A0+6] = B[A0+5] ^ B[A1+5] ^ B[A1+6] ^ B[A2+6] ^ B[A3+6]             ^ of2;
    Bp[A0+7] = B[A0+6] ^ B[A1+6] ^ B[A1+7] ^ B[A2+7] ^ B[A3+7];

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


