
#include <string.h>
#include <endian.h>
#include <stdio.h>
#include <assert.h>
#include "aes.h"
#include "bs.h"

void aes_ecb_encrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key)
{
    word_t input_space[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];

    memset(outputb,0,size);
    word_t * state = (word_t *)outputb;

    bs_expand_key(rk, key);

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
            memmove(state,inputb,BS_BLOCK_SIZE);
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
    
    bs_expand_key(rk, key);

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
            memmove(state,inputb,BS_BLOCK_SIZE);
            bs_cipher_rev(state,rk);
            size -= BS_BLOCK_SIZE;
            state += BS_BLOCK_SIZE;
        }

    }
}

static void INC_CTR(uint8_t * ctr)
{
    uint64_t * n = (uint64_t *) (ctr + (BLOCK_SIZE/8/2));
    *n = *n + 1;
    if (*n == 0)
    {
        n = (uint64_t *) ctr;
        *n = *n + 1;
    }
}

void aes_ctr_encrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key, uint8_t * iv)
{
    word_t input_space[BLOCK_SIZE];
    word_t rk[11][BLOCK_SIZE];
    word_t ctr[BLOCK_SIZE];
    
    memset(outputb,0,size);
    memset(ctr,0,sizeof(ctr));

    word_t * state = (word_t *)outputb;
    bs_expand_key(rk, key);

    int i,j=0;
    for (i = 0; i < BLOCK_SIZE; i += WORDS_PER_BLOCK)
    {
        memmove(ctr + i, iv, 16);
        INC_CTR((uint8_t *)(ctr+i));
    }
    for (i = 0; i < BLOCK_SIZE; i += WORDS_PER_BLOCK)
    {
        dump_hex((uint8_t*)(ctr+i),16);
    }


    bs_cipher(ctr, rk);

    assert(size < BS_BLOCK_SIZE);

    uint8_t * ctr_p = (uint8_t *) ctr;
    while(size--)
    {
        *outputb++ = *ctr_p++ ^ *inputb++;
    }

}


void aes_ctr_decrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key, uint8_t * iv)
{
}




