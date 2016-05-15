
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "aes.h"
#include "bs.h"
#include "utils.h"
#include "fr_defs.h"

#ifdef TEST_FOOTPRINT
#define printf(fmt, ...) (0)
#define fprintf(f,fmt, ...) (0)
#else
#include <stdio.h>
#endif

#define debug() __debug(__FILE__, __LINE__)
/*static void __debug(char * line, int num)*/
/*{*/
    /*fprintf(stderr, "%s: %d\n",line, num);*/
/*}*/

void dump_round(word_t * r,int roud);
void bs_mixcolumns_ref(word_t * B);

void aes_ecb_encrypt_ref(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key)
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

void aes_ecb_decrypt_ref(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key)
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

void aes_ctr_encrypt_ref(uint8_t * outputb, uint8_t * inputb, size_t size, uint8_t * key, uint8_t * iv,
        word_t (* rk)[BLOCK_SIZE])
{
    word_t ctr[BLOCK_SIZE];
    uint8_t iv_copy[BLOCK_SIZE/8];

    
    memset(outputb,0,size);
    memset(ctr,0,sizeof(ctr));
    memmove(iv_copy,iv,BLOCK_SIZE/8);

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

        uint8_t * ctr_p = (uint8_t *) ctr;
        size -= chunk;
        while(chunk--)
        {
            *outputb++ = *ctr_p++ ^ *inputb++;
        }

    }
    while(size);


}

static int num_rbits = (FR_STARTING_R_BITS+1);
static uint8_t rng_seed = 0x55;
static uint8_t ones_block[] = "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";
static uint8_t ones_enc[] = 
    "\xa9\x0f\x26\xc7\xf1\x48\xd9\x15\x58\x67\xb8\xd6\x6d\x58\x60\x1e";
static int iterations_encrypted = 0;

//                          1,    2,    3,    4,    5,     6,     7,     8,     9,     10
uint16_t CONTROL_VALS[10] = {0x3d, 0x88, 0xc1, 0xfb, 0x131, 0x175, 0x1b4, 0x1f8, 0x231, 0x271};

uint8_t FR_ROUNDS = FR_STARTING_ROUNDS;
volatile word_t DATA_ERRORS = 0;
volatile word_t CONTROL_ERRORS = 0;
word_t CONTROL_SHIFT;


void fault_handler()
{
    DATA_ERRORS = 0;

    num_rbits += FR_INCREMENT;
    if (FR_ROUNDS < 10)
    {
        CONTROL_ERRORS = CONTROL_VALS[FR_ROUNDS];
        FR_ROUNDS++;
    }
    /*printf("blocks_encrypted: %d\n", blocks_encrypted);*/
    /*printf("iterations_encrypted: %d\n", iterations_encrypted);*/
    if (num_rbits > 20)
    {
        printf("iterations_encrypted: %d\n", iterations_encrypted);
        fprintf(stderr, "over 20 faults detected, giving up!\n");
        exit(2);
    }
}

word_t fr_get_mask()
{
    word_t mask = 0;
    int i = num_rbits;
    int evenbits = (num_rbits + ((~num_rbits) & 1));

    rng_seed |= (~rng_seed) & 1;

    do
    {
        mask |= (ONE << (rng_seed & BS_2_MASK));
        int j = evenbits;
        rng_seed +=  (j<<1);
    }
    while(--i);

    return mask;
}

#define fr_seed_mask(seed) (rng_seed ^= (seed))

static void check_dfault(word_t * state, uint8_t * ciphertext)
{
    word_t redun[WORDS_PER_BLOCK];
    memset(redun,0,sizeof(redun));
    bs_get_slice(state, redun);
    if (memcmp(redun, ciphertext - 16, 16) != 0)
    {
        printf("D FAULT!\n");
    }
}
static void check_cfault(word_t * state)
{
    word_t redun[WORDS_PER_BLOCK];
    memset(redun,0,sizeof(redun));
    bs_get_slice(state, redun);
    if (memcmp(redun, ones_enc, 16) != 0)
    {
        printf("C FAULT!\n");
        dump_hex((uint8_t*)redun, 16);
    }
}

void aes_ctr_encrypt(uint8_t * outputb, uint8_t * inputb, int size, uint8_t * key, uint8_t * iv, word_t * rk)
{
    word_t iv_copy[WORDS_PER_BLOCK];
    word_t state[BLOCK_SIZE];
    word_t block_tmp[WORDS_PER_BLOCK];
    
    memset(outputb,0,size);
    memmove(iv_copy,iv,BLOCK_SIZE/8);
    memmove(block_tmp,iv,BLOCK_SIZE/8);

    int offset = 0;

    int blocks = size / (BLOCK_SIZE/8);
    memset(state, 0, sizeof(state));
    if (size % (BLOCK_SIZE/8))
    {
        blocks++;
    }

    int i,j = 0;

    // run pipeline
    for (i = 0; i < blocks; i++)
    {

        if (i > (BS_DATA_ROUNDS-1))
        {
            // 13 cycles/byte
            check_cfault(state);
        }

        for (j=0; j < WORDS_PER_BLOCK; j++)
        {
            block_tmp[j] = iv_copy[j] ^ ((word_t*)key)[j];
        }
        INC_CTR((uint8_t *)iv_copy,1);

        // 19 cycles/byte
        bs_add_slice(state, (word_t *)ones_block,1);

        if (i > (BS_DATA_ROUNDS-1))
        {
            bs_get_slice(state, (word_t*)(outputb + offset));
            offset += 16;
        }

        bs_add_slice(state, block_tmp,1);

        if (i > (BS_DATA_ROUNDS-1))
        {
            // 13 cycles/byte
            check_dfault(state, outputb + offset);
        }


        bs_add_slice(state, block_tmp,1);

        // 84 cycles/byte
        bs_apply_sbox(state);
        // 9 cycles/byte
        bs_shiftrows(state);
        // 38 cycles/byte
        bs_mixcolumns(state);

        // 9 cycles/byte
        bs_addroundkey(state,rk);

    }

    int leftover = MIN(i,BS_DATA_ROUNDS);

    // advance pipeline if it is not full
    for (; i < BS_DATA_ROUNDS; i++)
    {
        bs_add_slice(state, NULL,1);
        bs_add_slice(state, NULL,1);
        bs_add_slice(state, NULL,1);

        bs_apply_sbox(state);
        bs_shiftrows(state);
        bs_mixcolumns(state);

        bs_addroundkey(state,rk);
    }

    // empty pipeline
    for (j=0; j<leftover; j++)
    {

        {
            check_cfault(state);
        }

        bs_add_slice(state, NULL,1);

        bs_get_slice(state, (word_t*)(outputb + offset));
        offset += 16;

        bs_add_slice(state, NULL,1);
        {
            // 13 cycles/byte
            check_dfault(state, outputb + offset);
        }
        bs_add_slice(state, NULL,1);

        bs_apply_sbox(state);
        bs_shiftrows(state);
        bs_mixcolumns(state);
        bs_addroundkey(state,rk);

    }

    while(offset--)
    {
        outputb[offset] ^= inputb[offset];
    }

}




