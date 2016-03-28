#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "bs.h"


// transpose a block
// TODO do it in place
void bs_transpose(WORD * transpose, WORD * blocks)
{
    int i,j,k;
    WORD w;
    for(k=0; k < WORD_SIZE; k++)
    {
        for (i=0; i < WORDS_PER_BLOCK; i++)
        {
            w = blocks[k * WORDS_PER_BLOCK + i];
            for(j=0; j < WORD_SIZE; j++)
            {
                // TODO make const time
                transpose[i*WORD_SIZE + j] |= (w & (ONE << j)) ? (ONE<<k) : 0;
            }
        }
    }
}

void bs_transpose_rev(WORD * transpose, WORD * blocks)
{
    int i,j,k;
    WORD w;
    for(k=0; k < BLOCK_SIZE; k++)
    {
        w = blocks[k];
        for(j=0; j < WORD_SIZE; j++)
        {
            uint64_t bit = (w & (ONE << j)) ? (ONE << (k % WORD_SIZE)) : 0;
            // TODO make const time
            if ( k >= WORD_SIZE )
            {
                transpose[j * 2 + 1] |= bit;
            }
            else
            {
                transpose[j * 2] |= bit;
            }
        }
    }
}

void bs_dump(WORD * blocks)
{
    int i;
    for (i=0; i < BLOCK_SIZE; i++)
    {
        printf("%08"PRIx64"\n",(blocks[i]));
    }
}

void hex_dump(uint8_t * h, int len)
{
    while(len--)
        printf("%02hhx",*h++);
    printf("\n");
}

extern uint8_t INPUT[WORD_SIZE/8][BLOCK_SIZE/8 + 1];

int main()
{
    WORD blocks[ BLOCK_SIZE ];
    WORD blocks_tmp[ BLOCK_SIZE ];
    memset(blocks,0, sizeof(blocks));
    memset(blocks_tmp,0, sizeof(blocks));
    bs_transpose(blocks,(WORD*)INPUT);
    bs_transpose_rev(blocks_tmp,blocks);
    
    printf("original:\n");
    bs_dump((WORD*)INPUT);

    printf("transpose:\n");
    bs_dump(blocks);

    printf("double transpose:\n");
    bs_dump(blocks_tmp);


    return 0;
}


