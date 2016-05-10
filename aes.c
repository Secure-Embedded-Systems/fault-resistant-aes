
#include <string.h>
#include <stdio.h>
#include "aes.h"
#include "bs.h"
#include "utils.h"

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

static void INC_CTR(uint8_t * ctr, uint8_t i)
{
    ctr += BLOCK_SIZE/8 - 1;
    uint8_t n = *(ctr);
    *ctr += i;
    while(*ctr < n)
    {
        ctr--;
        n = *ctr;
        (*ctr)++;
    }
}

void aes_ctr_encrypt(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key, uint8_t * iv)
{
    word_t rk[11][BLOCK_SIZE];
    word_t ctr[BLOCK_SIZE];
    uint8_t iv_copy[BLOCK_SIZE/8];
    
    memset(outputb,0,size);
    memset(ctr,0,sizeof(ctr));
    memmove(iv_copy,iv,BLOCK_SIZE/8);

    word_t * state = (word_t *)outputb;
    bs_expand_key(rk, key);

    do
    {
        int chunk = MIN(size, BS_BLOCK_SIZE);
        int blocks = chunk / (BLOCK_SIZE/8);
        if (chunk % (BLOCK_SIZE/8))
        {
            blocks++;
        }

        int i;
        for (i = 0; i < blocks; i++)
        {
            memmove(ctr + (i * WORDS_PER_BLOCK), iv_copy, BLOCK_SIZE/8);
            INC_CTR(iv_copy,1);
        }

        bs_cipher(ctr, rk);
        size -= chunk;

        uint8_t * ctr_p = (uint8_t *) ctr;
        while(chunk--)
        {
            *outputb++ = *ctr_p++ ^ *inputb++;
        }

    }
    while(size);

}

static int num_rbits = 2;
static uint8_t rng_seed = 0x55;
static uint8_t ones_block[] = "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";
static uint8_t ones_enc[] = "\x8a\xf2\x86\x01\x42\xf7\x86\xf4\x09\x30\x7c\x1a\x3f\x7e\xaa\xac";

void fault_handler()
{
    num_rbits++;
}

word_t fr_get_mask()
{
    uint32_t mask = 0;
    int i = num_rbits;
    /*printf("seed: %x\n", rng_seed);*/

    rng_seed |= (~rng_seed) & 1;

    do
    {

        mask |= (1 << (rng_seed & BS_2_MASK));
        /*printf("mask: %x (|= %x)\n", mask, 1 << (rng_seed & BS_2_MASK));*/
        int j = (num_rbits + ((~num_rbits) & 1));
        do
        {
            rng_seed += 2;
        }
        while(--j);
    }
    while(--i);

    return mask;
}

void fr_seed_mask(uint8_t seed)
{
    rng_seed ^= seed;
}

#define debug() __debug(__FILE__, __LINE__)
static void __debug(char * line, int num)
{
    fprintf(stderr, "%s: %d\n",line, num);
}

void aes_ctr_encrypt_fr(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key, uint8_t * iv)
{
    word_t rk[11][BLOCK_SIZE];
    word_t ctr[BLOCK_SIZE];
    uint8_t iv_copy[BLOCK_SIZE/8];
    
    memset(outputb,0,size);
    memset(ctr,0,sizeof(ctr));
    memmove(iv_copy,iv,BLOCK_SIZE/8);

    word_t * state = (word_t *)outputb;
    bs_expand_key(rk, key);

    fr_seed_mask(inputb[size-1]);


    do
    {
        int chunk = MIN(size, BS_BLOCK_SIZE);
        int blocks = chunk / (BLOCK_SIZE/8);
        int processed = 0;
        uint8_t moved_const = 0;
        word_t frmask = fr_get_mask();

        /*printf("MASK: %x\n",frmask);*/

        if (chunk % (BLOCK_SIZE/8))
        {
            blocks++;
        }

        int i,j=0;
        memmove(ctr, iv_copy, BLOCK_SIZE/8);
        for (i = 1; i < WORD_SIZE; i++)
        {
            if (j >= blocks)
            {
                break;
            }
            // protected stream
            if (frmask & (1<<i))
            {
                if(!moved_const)
                {
                    memmove(ctr + (i * WORDS_PER_BLOCK), ones_block, BLOCK_SIZE/8);
                    moved_const = 1;
                }
                else
                {
                    memmove(ctr + (i * WORDS_PER_BLOCK), iv_copy, BLOCK_SIZE/8);
                }
            }
            else
            {
                INC_CTR(iv_copy,1);
                processed += BLOCK_SIZE/8;
                j++;
                memmove(ctr + (i * WORDS_PER_BLOCK), iv_copy, BLOCK_SIZE/8);
            }
        }

        bs_cipher(ctr, rk);

        fr_seed_mask(ctr[0]);

        // TODO consider reseeding here is necessary

        size -= processed;

        uint8_t * ctr_p = (uint8_t *) ctr;

        
        for(i = 0; i < processed; i++)
        {
            *outputb++ = *ctr_p++ ^ *inputb++;
            if ((i+1) % (BLOCK_SIZE/8) ==0 )
            {
                frmask >>= 1;
                if (frmask & 1)
                {
                    if (moved_const)
                    {
                        moved_const = 0;
                        if (memcmp(ctr_p, ones_enc, BLOCK_SIZE/8) != 0)
                        {
                            printf("control fault in %d!!\n", (i+1)/(BLOCK_SIZE/8));
                            dump_hex(ctr_p, BLOCK_SIZE/8);
                            fault_handler();
                        }
                    }
                    else
                        if (memcmp(ctr_p, ctr_p - BLOCK_SIZE/8, BLOCK_SIZE/8) != 0)
                    {
                        printf("faulty slice %d!!\n", (i+1)/(BLOCK_SIZE/8));
                        fault_handler();
                    }
                    else
                    {
                        printf("slice %d is ok\n",  (i+1)/(BLOCK_SIZE/8));
                    }
                    ctr_p += (BLOCK_SIZE/8);
                    frmask >>= 1;
                }
            }
        }
        printf("i:%d\n",i);

    }
    while(size > 0);

}




