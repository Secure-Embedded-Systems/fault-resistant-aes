#ifndef _BS_H_
#define _BS_H_

#include <stdint.h>

#define BLOCK_SIZE      128
#define WORD_SIZE       64
#define WORDS_PER_BLOCK  (BLOCK_SIZE / WORD_SIZE)

#if (WORD_SIZE==64)
    typedef uint64_t WORD;
#elif (WORD_SIZE==32)
    typedef uint32_t WORD;
#elif (WORD_SIZE==16)
    typedef uint16_t WORD;
#elif (WORD_SIZE==8)
    typedef uint8_t WORD;
#endif


#endif
