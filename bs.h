#ifndef _BS_H_
#define _BS_H_

#include <stdint.h>
#include <inttypes.h>

#define BLOCK_SIZE      128
#define WORD_SIZE       64
#define WORDS_PER_BLOCK  (BLOCK_SIZE / WORD_SIZE)

#if (WORD_SIZE==64)
    typedef uint64_t    word_t;
    #define ONE         1ULL
    #define WFMT        PRIx64
    #define WPAD        "016"
#elif (WORD_SIZE==32)
    typedef uint32_t    word_t;
    #define ONE         1UL
    #define WFMT        PRIx32
    #define WPAD        "08"
#elif (WORD_SIZE==16)
    typedef uint16_t    word_t;
    #define ONE         1
    #define WFMT        PRIx16
    #define WPAD        "04"
#elif (WORD_SIZE==8)
    typedef uint8_t     word_t;
    #define ONE         1
    #define WFMT        PRIx8
    #define WPAD        "02"
#else
#error "invalid word size"
#endif

void bs_transpose(word_t * transpose, word_t * blocks);
void bs_transpose_rev(word_t * transpose, word_t * blocks);

void bs_sbox(word_t S[8], word_t U[8]);
void bs_sbox_rev(word_t W[8], word_t U[8]);

void bs_shiftrow();
void bs_shiftrow_rev();

void bs_mixcolumns(word_t * Bp, word_t * B);
void bs_mixcolumns_rev();

void bs_addroundkey();
void bs_addroundkey_rev();

#endif
