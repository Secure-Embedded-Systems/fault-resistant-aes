#ifndef _BS_H_
#define _BS_H_

#include <stdint.h>

#define BLOCK_SIZE      128
#define WORD_SIZE       64
#define WORDS_PER_BLOCK  (BLOCK_SIZE / WORD_SIZE)

#if (WORD_SIZE==64)
    typedef uint64_t WORD;
    #define ONE      1ULL
#elif (WORD_SIZE==32)
    typedef uint32_t WORD;
    #define ONE      1UL
#elif (WORD_SIZE==16)
    typedef uint16_t WORD;
    #define ONE      1
#elif (WORD_SIZE==8)
    typedef uint8_t WORD;
    #define ONE      1
#else
#error "invalid word size"
#endif


#endif
