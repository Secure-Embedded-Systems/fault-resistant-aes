#include <stdio.h>
#include <stdlib.h>
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

void aes_ecb_encrypt(word_t * input, word_t ** rk)
{
    int round;

    bs_transpose(input);

    bs_addroundkey(input,rk[0]);

    for (round = 1; round < 10; round++)
    {
        bs_apply_sbox(input);
        bs_shiftrows(input);
        bs_mixcolumns(input);
        bs_addroundkey(input,rk[round]);
    }

    bs_apply_sbox(input);
    bs_shiftrows(input);
    bs_addroundkey(input,rk[10]);
    bs_transpose_rev(input);
}

void aes_ecb_decrypt(word_t * input, word_t ** rk)
{
    int round;

    bs_transpose(input);

    bs_addroundkey(input,rk[10]);

    for (round = 9; round > 0; round--)
    {
        bs_shiftrows_rev(input);
        bs_apply_sbox_rev(input);
        bs_addroundkey(input,rk[round]);
        bs_mixcolumns_rev(input);
    }
    bs_shiftrows_rev(input);
    bs_apply_sbox_rev(input);

    bs_addroundkey(input,rk[0]);

    bs_transpose_rev(input);
}


int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,"usage: %s <input-file>\n", argv[0]);
        exit(1);
    }

    return 0;
}


