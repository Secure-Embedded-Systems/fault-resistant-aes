#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bs.h"

#define bs_dump(x) dump_word(x, BLOCK_SIZE)
#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

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

void aes_ecb_encrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key)
{
    word_t input_space[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];

    memset(outputb,0,size);
    word_t * state = (word_t *)outputb;

    memmove(rk[0], key, BLOCK_SIZE/8);
    bs_expand_key(rk, (uint8_t *)rk[0]);

    while (size > 0)
    {
        if (size < BS_BLOCK_SIZE)
        {
            memset(input_space,0,BS_BLOCK_SIZE);
            memmove(input_space, inputb, size);
            bs_cipher(input_space,rk);
            memmove(outputb, input_space, size);
            size = 0;
            state += size;
        }
        else
        {
            bs_cipher(state,rk);
            size -= BS_BLOCK_SIZE;
            state += BS_BLOCK_SIZE;
        }

    }
}

void aes_ecb_decrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key)
{
    word_t input_space[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];

    memset(outputb,0,size);
    word_t * state = (word_t *)outputb;
    
    memmove(rk[0], key, BLOCK_SIZE/8);
    bs_expand_key(rk, (uint8_t *)rk[0]);

    while (size > 0)
    {
        if (size < BS_BLOCK_SIZE)
        {
            memset(input_space,0,BS_BLOCK_SIZE);
            memmove(input_space, inputb, size);
            bs_cipher_rev(input_space,rk);
            memmove(outputb, input_space, size);
            size = 0;
            state += size;
        }
        else
        {
            bs_cipher_rev(state,rk);
            size -= BS_BLOCK_SIZE;
            state += BS_BLOCK_SIZE;
        }

    }
}

// TODO AES CTR
int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        fprintf(stderr,"usage: %s <input-file>\n", argv[0]);
        exit(1);
    }
    uint8_t key_vector[16] = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";
    uint8_t pt_vector[16] =  "\x6b\xc1\xbe\xe2\x2e\x40\x9f\x96\xe9\x3d\x7e\x11\x73\x93\x17\x2a";
    uint8_t output[16];
    uint8_t input[16];

    uint8_t key_s[KEY_SCHEDULE_SIZE];
    memmove(input, pt_vector, sizeof(pt_vector));

    aes_ecb_encrypt(output, pt_vector,16,key_vector);


    printf("cipher text: \n");
    dump_hex(output, 16);

    aes_ecb_decrypt(input, output, 16, key_vector);

    printf("plain text: \n");
    dump_hex((uint8_t * )input,16);

    return 0;
}


