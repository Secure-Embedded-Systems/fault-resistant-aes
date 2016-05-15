
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

static uint8_t ones_block[] = "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff";
static uint8_t ones_enc[] = 
    "\xa9\x0f\x26\xc7\xf1\x48\xd9\x15\x58\x67\xb8\xd6\x6d\x58\x60\x1e";
static int iterations_encrypted = 0;

//                          1,    2,    3,    4,    5,     6,     7,     8,     9,     10

volatile word_t DATA_ERRORS = 0;
volatile word_t CONTROL_ERRORS = 0;


void fault_handler()
{
    DATA_ERRORS = 0;

    if (FR_ROUNDS < 10)
    {
        CONTROL_ERRORS = CONTROL_VALS[FR_ROUNDS];
        FR_ROUNDS++;
    }
    /*printf("blocks_encrypted: %d\n", blocks_encrypted);*/
    printf("iterations_encrypted: %d\n", iterations_encrypted);
}

#define INIT_SHIFT 0

#define BS_DATA_MASK_UNSHIFTED 0x7ffffff
word_t BS_DATA_MASK = BS_DATA_MASK_UNSHIFTED;

int ROOT_BIT = 29 + INIT_SHIFT;
int START_BIT = 0;

int BIT_CFAULT = ((29) % WORD_SIZE);
int BIT_DFAULT = ((29 - 2) % WORD_SIZE);
int BIT_DATA   = ((29 - 1) % WORD_SIZE);



static void check_dfault(word_t * state, uint8_t * ciphertext)
{
    word_t redun[WORDS_PER_BLOCK];
    memset(redun,0,sizeof(redun));
    bs_get_slice(state, redun, BIT_DFAULT);
    if (memcmp(redun, ciphertext - 16, 16) != 0)
    {
        printf("D FAULT!\n");
        dump_hex((uint8_t*)redun, 16);
    }
}

static void check_cfault(word_t * state)
{
    word_t redun[WORDS_PER_BLOCK];
    memset(redun,0,sizeof(redun));
    bs_get_slice(state, redun, BIT_CFAULT);
    if (memcmp(redun, ones_enc, 16) != 0)
    {
        printf("C FAULT!\n");
        dump_hex((uint8_t*)redun, 16);
    }
}

void shift_rk(word_t * rk, int shift)
{
    int i;
    for(i = 0; i < BLOCK_SIZE; i++)
    {
        rk[i] = rotl(rk[i],shift);
    }

    BS_DATA_MASK = rotl(BS_DATA_MASK,shift);

    START_BIT += shift;
    START_BIT %= WORD_SIZE;

    BIT_CFAULT += shift;
    BIT_CFAULT %= WORD_SIZE;

    BIT_DFAULT += shift;
    BIT_DFAULT %= WORD_SIZE;

    BIT_DATA += shift;
    BIT_DATA %= WORD_SIZE;

    /*CURRENT_SHIFT = shift;*/
    /*CURRENT_SHIFT %= WORD_SIZE;*/
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
    shift_rk(rk, inputb[0] & 0x1f);

    int i,j = 0;

    // run pipeline
    for (i = 0; i < blocks; i++)
    {

        if (i > (BS_DATA_ROUNDS-1))
        {
            // 13 cycles/byte
            check_cfault(state);
            bs_get_slice(state, (word_t*)(outputb + offset),BIT_DATA);
            offset += 16;
            check_dfault(state, outputb + offset);
        }

        for (j=0; j < WORDS_PER_BLOCK; j++)
        {
            block_tmp[j] = iv_copy[j] ^ ((word_t*)key)[j];
        }
        INC_CTR((uint8_t *)iv_copy,1);

        // 19 cycles/byte for each
        bs_add_slice(state, (word_t *)ones_block,START_BIT);
        bs_add_slice(state, block_tmp,START_BIT);
        bs_add_slice(state, block_tmp,START_BIT);

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
        bs_add_slice(state, NULL,START_BIT);
        bs_add_slice(state, NULL,START_BIT);
        bs_add_slice(state, NULL,START_BIT);

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
            bs_get_slice(state, (word_t*)(outputb + offset), BIT_DATA);
            offset += 16;
            check_dfault(state, outputb + offset);
        }

        bs_add_slice(state, NULL,START_BIT);
        bs_add_slice(state, NULL,START_BIT);
        bs_add_slice(state, NULL,START_BIT);

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




