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

void aes_ecb_encrypt(word_t * input, word_t (* rk)[BLOCK_SIZE])
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

void aes_ecb_decrypt(word_t * input, word_t (* rk)[BLOCK_SIZE])
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
    uint8_t key_vector[16] = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";
    uint8_t pt_vector[16] =  "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a";

    word_t input[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    uint8_t key_s[KEY_SCHEDULE_SIZE];
    memset(key_s, 0, sizeof(key_s));
    memset(input, 0, sizeof(input));
    memmove(key_s, key_vector, sizeof(key_vector));
    memmove(input, pt_vector, sizeof(pt_vector));

    bs_expand_key(rk, key_s);
    aes_ecb_encrypt(input,rk);

    printf("cipher text: \n");
    dump_hex((uint8_t * )input,16);

    aes_ecb_decrypt(input,rk);

    printf("plain text: \n");
    dump_hex((uint8_t * )input,16);

    return 0;
}


