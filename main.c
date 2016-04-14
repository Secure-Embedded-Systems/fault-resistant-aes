#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bs.h"

#define bs_dump(x) dump_word(x, BLOCK_SIZE)

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

void bs_expand_key(word_t (* rk)[BLOCK_SIZE], uint8_t * key)
{
    // TODO integrate this better
    expand_key(key);

    int i, j = 0, k, l;
    for (i = 0; i < KEY_SCHEDULE_SIZE; i += (BLOCK_SIZE/8))
    {
        memmove(rk[j], key + i, BLOCK_SIZE / 8);

        for (k = WORDS_PER_BLOCK; k < 128; k += WORDS_PER_BLOCK)
        {
            for (l = 0; l < WORDS_PER_BLOCK; l++)
            {
                rk[j][k + l] = rk[j][l];
            }
        }
        bs_transpose(rk[j]);
        j++;
    }

}

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,"usage: %s <input-file>\n", argv[0]);
        exit(1);
    }
    uint8_t key[16] = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
    uint8_t key2[16] = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f";

    word_t rk[11][BLOCK_SIZE];
    uint8_t key_s[KEY_SCHEDULE_SIZE];
    memset(key_s, 0, sizeof(key_s));
    memmove(key_s, key2, sizeof(key));

    bs_expand_key(rk, key_s);



    int i,j;
    for (i=0; i < 11; i++)
    {
        bs_transpose_rev(rk[i]);

        for (j=0; j < 11; j++)
        {
            dump_hex((uint8_t *)rk[i] + j * 16, 16);
        }
    }

    return 0;
}


