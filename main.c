#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "bs.h"

/*July 2011*/
/*Straight-line program for AES s box*/

/*Input is U[0], U[1],...,U[7]*/
/*Output is S[0], S[1],...,S[7]*/
// http://cs-www.cs.yale.edu/homes/peralta/CircuitStuff/CMT.html
void bs_sbox_rev(uint8_t W[8], uint8_t U[8])
{
    uint8_t
        T1,T2,T3,T4,T5,T6,T8,
        T9,T10,T13,T14,T15,T16,
        T17,T18,T19,T20,T22,T23,T24,
        T25, T26, T27;

    uint8_t 
        M1,M2,M3,M4,M5,M6,M7,M8,
        M9,M10,M11,M12,M13,M14,M15,
        M16,M17,M18,M19,M20,M21,M22,
        M23,M24,M25,M26,M27,M28,M29,
        M30,M31,M32,M33,M34,M35,M36,
        M37,M38,M39,M40,M41,M42,M43,
        M44,M45,M46,M47,M48,M49,M50,
        M51,M52,M53,M54,M55,M56,M57,
        M58,M59,M60,M61,M62,M63;

    uint8_t
        P0,P1,P2,P3,P4,P5,P6,P7,P8,
        P9,P10,P11,P12,P13,P14,
        P15,P16,P17,P18,P19,P20,
        P21,P22,P23,P24,P25,P26,
        P27,P28,P29;

    uint8_t Y5,
        R5, R13, R17, R18, R19;


    T23 = U[0] ^ U[3];
    T22 = ~(U[1] ^ U[3]);
    T2 = ~(U[0] ^ U[1]);
    T1 = U[3] ^ U[4];
    T24 = ~(U[4] ^ U[7]);
    R5 = U[6] ^ U[7];
    T8 = ~(U[1] ^ T23);
    T19 = T22 ^ R5;
    T9 = ~(U[7] ^ T1);
    T10 = T2 ^ T24;
    T13 = T2 ^ R5;
    T3 = T1 ^ R5;
    T25 = ~(U[2] ^ T1);
    R13 = U[1] ^ U[6];
    T17 = ~(U[2] ^ T19);
    T20 = T24 ^ R13;
    T4 = U[4] ^ T8;
    R17 = ~(U[2] ^ U[5]);
    R18 = ~(U[5] ^ U[6]);
    R19 = ~(U[2] ^ U[4]);
    Y5 = U[0] ^ R17;
    T6 = T22 ^ R17;
    T16 = R13 ^ R19;
    T27 = T1 ^ R18;
    T15 = T10 ^ T27;
    T14 = T10 ^ R18;
    T26 = T3 ^ T16;
    M1 = T13 & T6;
    M2 = T23 & T8;
    M3 = T14 ^ M1;
    M4 = T19 & Y5;
    M5 = M4 ^ M1;
    M6 = T3 & T16;
    M7 = T22 & T9;
    M8 = T26 ^ M6;
    M9 = T20 & T17;
    M10 = M9 ^ M6;
    M11 = T1 & T15;
    M12 = T4 & T27;
    M13 = M12 ^ M11;
    M14 = T2 & T10;
    M15 = M14 ^ M11;
    M16 = M3 ^ M2;
    M17 = M5 ^ T24;
    M18 = M8 ^ M7;
    M19 = M10 ^ M15;
    M20 = M16 ^ M13;
    M21 = M17 ^ M15;
    M22 = M18 ^ M13;
    M23 = M19 ^ T25;
    M24 = M22 ^ M23;
    M25 = M22 & M20;
    M26 = M21 ^ M25;
    M27 = M20 ^ M21;
    M28 = M23 ^ M25;
    M29 = M28 & M27;
    M30 = M26 & M24;
    M31 = M20 & M23;
    M32 = M27 & M31;
    M33 = M27 ^ M25;
    M34 = M21 & M22;
    M35 = M24 & M34;
    M36 = M24 ^ M25;
    M37 = M21 ^ M29;
    M38 = M32 ^ M33;
    M39 = M23 ^ M30;
    M40 = M35 ^ M36;
    M41 = M38 ^ M40;
    M42 = M37 ^ M39;
    M43 = M37 ^ M38;
    M44 = M39 ^ M40;
    M45 = M42 ^ M41;
    M46 = M44 & T6;
    M47 = M40 & T8;
    M48 = M39 & Y5;
    M49 = M43 & T16;
    M50 = M38 & T9;
    M51 = M37 & T17;
    M52 = M42 & T15;
    M53 = M45 & T27;
    M54 = M41 & T10;
    M55 = M44 & T13;
    M56 = M40 & T23;
    M57 = M39 & T19;
    M58 = M43 & T3;
    M59 = M38 & T22;
    M60 = M37 & T20;
    M61 = M42 & T1;
    M62 = M45 & T4;
    M63 = M41 & T2;
    P0 = M52 ^ M61;
    P1 = M58 ^ M59;
    P2 = M54 ^ M62;
    P3 = M47 ^ M50;
    P4 = M48 ^ M56;
    P5 = M46 ^ M51;
    P6 = M49 ^ M60;
    P7 = P0 ^ P1;
    P8 = M50 ^ M53;
    P9 = M55 ^ M63;
    P10 = M57 ^ P4;
    P11 = P0 ^ P3;
    P12 = M46 ^ M48;
    P13 = M49 ^ M51;
    P14 = M49 ^ M62;
    P15 = M54 ^ M59;
    P16 = M57 ^ M61;
    P17 = M58 ^ P2;
    P18 = M63 ^ P5;
    P19 = P2 ^ P3;
    P20 = P4 ^ P6;
    P22 = P2 ^ P7;
    P23 = P7 ^ P8;
    P24 = P5 ^ P7;
    P25 = P6 ^ P10;
    P26 = P9 ^ P11;
    P27 = P10 ^ P18;
    P28 = P11 ^ P25;
    P29 = P15 ^ P20;
    W[0] = P13 ^ P22;
    W[1] = P26 ^ P29;
    W[2] = P17 ^ P28;
    W[3] = P12 ^ P22;
    W[4] = P23 ^ P27;
    W[5] = P19 ^ P24;
    W[6] = P14 ^ P23;
    W[7] = P9 ^ P16;


}
void bs_sbox(uint8_t S[8], uint8_t U[8])
{

    uint8_t
        T1,T2,T3,T4,T5,T6,T7,T8,
        T9,T10,T11,T12,T13,T14,T15,T16,
        T17,T18,T19,T20,T21,T22,T23,T24,
        T25, T26, T27;

    uint8_t 
        M1,M2,M3,M4,M5,M6,M7,M8,
        M9,M10,M11,M12,M13,M14,M15,
        M16,M17,M18,M19,M20,M21,M22,
        M23,M24,M25,M26,M27,M28,M29,
        M30,M31,M32,M33,M34,M35,M36,
        M37,M38,M39,M40,M41,M42,M43,
        M44,M45,M46,M47,M48,M49,M50,
        M51,M52,M53,M54,M55,M56,M57,
        M58,M59,M60,M61,M62,M63;

    uint8_t
        L0,L1,L2,L3,L4,L5,L6,L7,L8,
        L9,L10,L11,L12,L13,L14,
        L15,L16,L17,L18,L19,L20,
        L21,L22,L23,L24,L25,L26,
        L27,L28,L29;

    T1 = U[0] ^ U[3];
        T2 = U[0] ^ U[5];
        T3 = U[0] ^ U[6];
        T4 = U[3] ^ U[5];
        T5 = U[4] ^ U[6];
        T6 = T1 ^ T5;
        T7 = U[1] ^ U[2];
        T8 = U[7] ^ T6;
        T9 = U[7] ^ T7;
        T10 = T6 ^ T7;
        T11 = U[1] ^ U[5];
        T12 = U[2] ^ U[5];
        T13 = T3 ^ T4;
        T14 = T6 ^ T11;
        T15 = T5 ^ T11;
        T16 = T5 ^ T12;
        T17 = T9 ^ T16;
        T18 = U[3] ^ U[7];
        T19 = T7 ^ T18;
        T20 = T1 ^ T19;
        T21 = U[6] ^ U[7];
        T22 = T7 ^ T21;
        T23 = T2 ^ T22;
        T24 = T2 ^ T10;
        T25 = T20 ^ T17;
        T26 = T3 ^ T16;
        T27 = T1 ^ T12;
        M1 = T13 & T6;
        M2 = T23 & T8;
        M3 = T14 ^ M1;
        M4 = T19 & U[7];
        M5 = M4 ^ M1;
        M6 = T3 & T16;
        M7 = T22 & T9;
        M8 = T26 ^ M6;
        M9 = T20 & T17;
        M10 = M9 ^ M6;
        M11 = T1 & T15;
        M12 = T4 & T27;
        M13 = M12 ^ M11;
        M14 = T2 & T10;
        M15 = M14 ^ M11;
        M16 = M3 ^ M2;
        M17 = M5 ^ T24;
        M18 = M8 ^ M7;
        M19 = M10 ^ M15;
        M20 = M16 ^ M13;
        M21 = M17 ^ M15;
        M22 = M18 ^ M13;
        M23 = M19 ^ T25;
        M24 = M22 ^ M23;
        M25 = M22 & M20;
        M26 = M21 ^ M25;
        M27 = M20 ^ M21;
        M28 = M23 ^ M25;
        M29 = M28 & M27;
        M30 = M26 & M24;
        M31 = M20 & M23;
        M32 = M27 & M31;
        M33 = M27 ^ M25;
        M34 = M21 & M22;
        M35 = M24 & M34;
        M36 = M24 ^ M25;
        M37 = M21 ^ M29;
        M38 = M32 ^ M33;
        M39 = M23 ^ M30;
        M40 = M35 ^ M36;
        M41 = M38 ^ M40;
        M42 = M37 ^ M39;
        M43 = M37 ^ M38;
        M44 = M39 ^ M40;
        M45 = M42 ^ M41;
        M46 = M44 & T6;
        M47 = M40 & T8;
        M48 = M39 & U[7];
        M49 = M43 & T16;
        M50 = M38 & T9;
        M51 = M37 & T17;
        M52 = M42 & T15;
        M53 = M45 & T27;
        M54 = M41 & T10;
        M55 = M44 & T13;
        M56 = M40 & T23;
        M57 = M39 & T19;
        M58 = M43 & T3;
        M59 = M38 & T22;
        M60 = M37 & T20;
        M61 = M42 & T1;
        M62 = M45 & T4;
        M63 = M41 & T2;
        L0 = M61 ^ M62;
        L1 = M50 ^ M56;
        L2 = M46 ^ M48;
        L3 = M47 ^ M55;
        L4 = M54 ^ M58;
        L5 = M49 ^ M61;
        L6 = M62 ^ L5;
        L7 = M46 ^ L3;
        L8 = M51 ^ M59;
        L9 = M52 ^ M53;
        L10 = M53 ^ L4;
        L11 = M60 ^ L2;
        L12 = M48 ^ M51;
        L13 = M50 ^ L0;
        L14 = M52 ^ M61;
        L15 = M55 ^ L1;
        L16 = M56 ^ L0;
        L17 = M57 ^ L1;
        L18 = M58 ^ L8;
        L19 = M63 ^ L4;
        L20 = L0 ^ L1;
        L21 = L1 ^ L7;
        L22 = L3 ^ L12;
        L23 = L18 ^ L2;
        L24 = L15 ^ L9;
        L25 = L6 ^ L10;
        L26 = L7 ^ L9;
        L27 = L8 ^ L10;
        L28 = L11 ^ L14;
        L29 = L11 ^ L17;
        S[0] = L6 ^ L24;
        S[1] = ~(L16 ^ L26);
        S[2] = ~(L19 ^ L28);
        S[3] = L6 ^ L21;
        S[4] = L20 ^ L22;
        S[5] = L25 ^ L29;
        S[6] = ~(L13 ^ L27);
        S[7] = ~(L6 ^ L23);

}

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
#if 0
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
#endif

    uint8_t sbox_in[] = {0x21,0x2,0x3,0x4,0x5,0x6,0x7,0x8};
    uint8_t sbox_out[8];
    uint8_t sbox_rev[8];
    int idx = 7;

    printf("SBOX input : ");
    hex_dump(sbox_in,8);

    bs_sbox((uint8_t*)sbox_out, (uint8_t*)sbox_in);
    
    printf("SBOX output : ");
    hex_dump(sbox_out,8);

    bs_sbox_rev((uint8_t*)sbox_rev, (uint8_t*)sbox_out);

    printf("SBOX reverse : ");
    hex_dump(sbox_rev,8);


    return 0;
}


