#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <asm-leon/timer.h>

#include "app.h"
#include "../bs.h"


int cli_app(int argc, char * argv[])
{
    uint8_t key[16] = "\x2b\x7e\x15\x16\x28\xae\xd2\xa6\xab\xf7\x15\x88\x09\xcf\x4f\x3c";
    uint8_t iv[16]  = "\xf0\xf1\xf2\xf3\xf4\xf5\xf6\xf7\xf8\xf9\xfa\xfb\xfc\xfd\xfe\xff";

    uint8_t pt[16*10000];
    uint8_t ct[16*10000];
    uint8_t ct2[16*10000];

    int amt = sizeof(pt);

    printf("%d bytes\n",amt);

    word_t rk[11][BLOCK_SIZE];
    bs_expand_key(rk, key);
    leonbare_init_ticks();

    uint32_t tstart,tend;
    tstart = clock();
    {
        aes_ctr_encrypt(ct, pt, amt, key, iv, rk);
    }
    tend = clock();

    double total = ((1.0e-6 * (double)tend) -
                (1.0e-6 * (double)tstart));

    printf("unprotected performance for %d word length\n", WORD_SIZE);
    printf("-------------------------------\n");
    printf("%.5f s\n", total);
    printf("%.15f s/byte\n", total/amt);
    printf("%.5f cycles/byte (for 50 MHz)\n", 50ull * (1ull<<20) * total/amt);

    
    tstart = clock();
    {
        aes_ctr_encrypt_fr(ct2, pt, amt, key, iv,rk);
    }
    tend = clock();

    total = ((1.0e-6 * (double)tend) -
            (1.0e-6 * (double)tstart));

    printf("protected performance for %d word length\n", WORD_SIZE);
    printf("-------------------------------\n");
    printf("%.5f s\n", total);
    printf("%.15f s/byte\n", total/amt);
    printf("%.5f cycles/byte (for 50 MHz)\n", 50ull * (1ull<<20) * total/amt);

    if (memcmp(ct, ct2, amt) != 0)
    {
        fprintf(stderr, "error: protect CTR does not match unprotected CTR\n");
        /*dump_hex(ct,amt);*/
        /*printf("\n");*/
        /*dump_hex(ct2,amt);*/
    }


    return 0;
}
